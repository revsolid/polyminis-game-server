#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "Core/Server.h"
#include "Game/GameUtils.h"

#include "OrbitalInteractionsService.h"

namespace SpaceExploration
{

        OrbitalInteractionsService::OrbitalInteractionsService(PolyminisServer::WSServer& server, PlanetManager& pManager,
                                                               PolyminisServer::ServerCfg almanacServerCfg) : mAlmanacServerCfg(almanacServerCfg) 
        {
            auto wss = std::make_shared<PolyminisServer::WSService>();
            wss->mServiceName = "orbital_interactions";
            wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sd)
                             {
                                 return this->OISEndpoint(request, sd);
                             };
            server.AddService(wss);
        }

        picojson::object OrbitalInteractionsService::OISEndpoint(picojson::value& request, PolyminisServer::SessionData& sd)
        {
            std::string command = JsonHelpers::json_get_string(request, "Command");
            auto payload = JsonHelpers::json_get_as_object(request);
            picojson::object toRet;
            toRet["Service"] = picojson::value("orbital_interactions");

            if (command == "EXTRACT") 
            {
                // TO EXTRACT: We update the proportion of the species in the DB and
                // update the Biomass of the player on the DB - To find the species we need
                // to extend the almanac to support an extra query param on get
                float percentageChange = JsonHelpers::json_get_float(request, "ExtractedPopulation");
                int   pid = JsonHelpers::json_get_int(request, "PlanetId");
                int   epoch = JsonHelpers::json_get_int(request, "Epoch");
                float toExtract = PopulationPercentageToBiomass(toExtract);

                std::string speciesName = JsonHelpers::json_get_string(request, "SpeciesName");

                // Update 
                try
                {

                    std::string url = "/persistence/speciessummaries/"+std::to_string(pid)+""+std::to_string(epoch)+"/"+speciesName;
                    picojson::object response = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                                                     PolyminisServer::HttpMethod::GET, picojson::object());

                    auto response_v = picojson::value(response);
                    auto species = JsonHelpers::json_get_object(response_v, "Response");
                    auto species_v = picojson::value(species);

                    float oldPercentage = JsonHelpers::json_get_float(species_v, "Percentage");

                    picojson::object xtractPayload;

                    if (oldPercentage < percentageChange)
                    {
                        // Error
                        std::cout << "Trying to substract more percentage than there is: " << oldPercentage << " - " << percentageChange << std::endl;
                        return JsonHelpers::json_create_error("Error - Trying to Substarct more Percentage than available");
                    }

                    xtractPayload["Percentage"] = picojson::value(oldPercentage - percentageChange);
                    PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                         PolyminisServer::HttpMethod::PUT, xtractPayload);
                    sd.BiomassAvailable += toExtract; 

                    GameUtils::SaveBiomassValue(sd, mAlmanacServerCfg); 
                    toRet["EventString"] = picojson::value("ExtractResult");
                    toRet["NewBiomassAvialble"] = picojson::value(sd.BiomassAvailable);
                }
                catch (websocketpp::exception const & e)
                {
                    std::cout << e.what() << std::endl;
                    return JsonHelpers::json_create_error("Error - Http Or WebSocket Error");
                }
            }
            else if (command == "EDIT_IN_PLANET")
            {
            }
            else if (command == "DEPLOY") 
            {
// TODO: GAME_RULES - Tagging al places that have Game rules implemented for later tracking 
                // TO DEPLOY: Insert the species in the planet in epoch with the proportion based on the invested Biomass.
                //
                //
                // Get All Species in PlanetEpoch
                // Substract Percentage from them
                // Add new species with percentage
                float toDeploy = JsonHelpers::json_get_float(request, "DeployedBiomass");
                int   pid = JsonHelpers::json_get_int(request, "PlanetId");
                int   epoch = JsonHelpers::json_get_int(request, "Epoch");
                std::string planetEpoch = std::to_string(pid)+std::to_string(epoch);
                float percentageForDeployed = BiomassToPopulationPercentage(toDeploy);
                std::string speciesName = JsonHelpers::json_get_string(request, "SpeciesName");

                try
                {
                    std::string url = "/persistence/speciessummaries/"+planetEpoch;
                    picojson::object response = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                                                     PolyminisServer::HttpMethod::GET, picojson::object());

                    if (toDeploy > sd.BiomassAvailable)
                    {
                        // Error
                        return JsonHelpers::json_create_error("Error - Trying to Deploy more Biomass than Available");
                    }

                    auto response_v = picojson::value(response);
                    auto species_arr = JsonHelpers::json_get_array(picojson::value(JsonHelpers::json_get_object(response_v, "Response")), "Items");

                    for (auto species_json : species_arr)
                    {
                        float oldPercentage = JsonHelpers::json_get_float(species_json, "Percentage");
                        float newPercentage = oldPercentage -  (oldPercentage * percentageForDeployed);

                        std::string sName = JsonHelpers::json_get_string(species_json, "SpeciesName");

                        picojson::object xtractPayload;
                        xtractPayload["Percentage"] = picojson::value(newPercentage);
                        url = "/persistence/speciessummaries/"+planetEpoch+"/"+sName;
                        PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                             PolyminisServer::HttpMethod::PUT, xtractPayload);
                    }

                    sd.BiomassAvailable -= toDeploy;
                    GameUtils::SaveBiomassValue(sd, mAlmanacServerCfg);
                    toRet["EventString"] = picojson::value("DeployResult");
                    toRet["NewBiomassAvialble"] = picojson::value(sd.BiomassAvailable);

                    picojson::object newSpeciesPayload;
                    newSpeciesPayload["Percentage"] = picojson::value(percentageForDeployed);
                    newSpeciesPayload["PlanetEpoch"] = picojson::value(planetEpoch);
                    newSpeciesPayload["SpeciesName"] = picojson::value(speciesName);
                    url = "/persistence/speciessummaries/"+planetEpoch+"/"+speciesName;
                    PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                         PolyminisServer::HttpMethod::POST, newSpeciesPayload);

                }
                catch (websocketpp::exception const & e)
                {
                    std::cout << e.what() << std::endl;
                    return JsonHelpers::json_create_error("Error - Http Or WebSocket Error");
                }
            }
            return toRet;
        }
}

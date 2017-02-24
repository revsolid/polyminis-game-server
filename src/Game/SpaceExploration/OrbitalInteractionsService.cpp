#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "Core/Server.h"
#include "Game/GameUtils.h"

#include "OrbitalInteractionsService.h"

namespace SpaceExploration
{

        OrbitalInteractionsService::OrbitalInteractionsService(PolyminisServer::WSServer& server, PlanetManager& pManager,
                                                               PolyminisServer::ServerCfg almanacServerCfg) : mAlmanacServerCfg(almanacServerCfg), mPlanetManager(pManager)
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

            int  pid = JsonHelpers::json_get_int(request, "PlanetId");
            int  epoch = JsonHelpers::json_get_int(request, "Epoch");
            std::string planetEpoch = std::to_string(pid)+std::to_string(epoch);
            bool reloadPlanet = false;

            if (command == "EXTRACT") 
            {
                // TO EXTRACT: We update the proportion of the species in the DB and
                // update the Biomass of the player on the DB - To find the species we need
                // to extend the almanac to support an extra query param on get
                float percentageChange = JsonHelpers::json_get_float(request, "ExtractedPopulation");
                float toExtract = PopulationPercentageToBiomass(toExtract);

                auto speciesJson = JsonHelpers::json_get_object(request, "Species");
                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");

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
                    toRet["EventString"] = picojson::value("EXTRACT_RESULT");
                    toRet["NewBiomassAvailable"] = picojson::value(sd.BiomassAvailable);
                    reloadPlanet = true;
                }
                catch (websocketpp::exception const & e)
                {
                    std::cout << e.what() << std::endl;
                    return JsonHelpers::json_create_error("Error - Http Or WebSocket Error");
                }
            }
            else if (command == "GET_TO_EDIT_IN_PLANET" || command == "SAMPLE_FROM_PLANET")
            {
                // Both of this commands mean 'Go get the full DNA from the DB for this Species'
                auto speciesJson = JsonHelpers::json_get_object(request, "Species");

                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");
                toRet["Species"] = picojson::value(GameUtils::GetSpeciesFullData(mAlmanacServerCfg, planetEpoch, speciesName));
                toRet["InPlanet"] = picojson::value((float)pid);

                if (command == "GET_TO_EDIT_IN_PLANET")
                {
                    toRet["EventString"] = picojson::value("GET_EDIT_RESULT");
                }
                else
                {
                    toRet["EventString"] = picojson::value("SAMPLE_DNA_RESULT");
                }
            }
            else if (command == "EDIT_IN_PLANET")
            {
                auto speciesJson = JsonHelpers::json_get_object(request, "Species");
                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");
                picojson::object newSpeciesPayload;
                newSpeciesPayload["Splices"] = picojson::value(JsonHelpers::json_get_array(picojson::value(speciesJson), "Splices"));
                std::string url = "/persistence/speciesinplanet/"+planetEpoch+"/"+speciesName;
                PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                     PolyminisServer::HttpMethod::PUT, newSpeciesPayload);

                toRet["NewBiomassAvailable"] = picojson::value(sd.BiomassAvailable);
                reloadPlanet = true;
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
                float percentageForDeployed = BiomassToPopulationPercentage(toDeploy);

                auto speciesJson = JsonHelpers::json_get_object(request, "Species");
                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");

                try
                {
                    if (toDeploy > sd.BiomassAvailable)
                    {
                        // Error
                        return JsonHelpers::json_create_error("Error - Trying to Deploy more Biomass than Available");
                    }

                    auto species_arr = GameUtils::GetSpeciesInPlanet(mAlmanacServerCfg, planetEpoch);

                    for (auto species_json : species_arr)
                    {
                        float oldPercentage = JsonHelpers::json_get_float(species_json, "Percentage");
                        float newPercentage = oldPercentage -  (oldPercentage * percentageForDeployed);

                        std::string sName = JsonHelpers::json_get_string(species_json, "SpeciesName");

                        picojson::object xtractPayload;
                        xtractPayload["Percentage"] = picojson::value(newPercentage);
                        std::string url = "/persistence/speciessummaries/"+planetEpoch+"/"+sName;
                        PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                             PolyminisServer::HttpMethod::PUT, xtractPayload);
                    }

                    sd.BiomassAvailable -= toDeploy;
                    GameUtils::SaveBiomassValue(sd, mAlmanacServerCfg);
                    toRet["EventString"] = picojson::value("DEPLOY_RESULT");
                    toRet["NewBiomassAvailable"] = picojson::value(sd.BiomassAvailable);

                    picojson::object newSpeciesPayload;
                    newSpeciesPayload["Percentage"] = picojson::value(percentageForDeployed);
                    newSpeciesPayload["PlanetEpoch"] = picojson::value(planetEpoch);
                    newSpeciesPayload["SpeciesName"] = picojson::value(speciesName);
                    newSpeciesPayload["CreatorName"] = picojson::value(sd.UserName);
                    newSpeciesPayload["Splices"] = picojson::value(JsonHelpers::json_get_array(picojson::value(speciesJson), "Splices"));
                    std::string url = "/persistence/speciesinplanet/"+planetEpoch+"/"+speciesName;
                    PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                         PolyminisServer::HttpMethod::POST, newSpeciesPayload);

                    reloadPlanet = true;
                }
                catch (websocketpp::exception const & e)
                {
                    std::cout << e.what() << std::endl;
                    return JsonHelpers::json_create_error("Error - Http Or WebSocket Error");
                }
            }


            if (reloadPlanet)
            {
                auto& planet = mPlanetManager.GetPlanet(pid);
                picojson::array species = GameUtils::GetSpeciesInPlanet(mAlmanacServerCfg, planetEpoch);
                planet.SwapSpecies(species);

                toRet["EventString"] = picojson::value("INTERACTION_RESULT");
                toRet["UpdatedPlanet"] = picojson::value(planet.ToJson());
                std::cout << "Reloading Planet... y'all" << std::endl;
            }

            std::cout << " Sending down Event: " << picojson::value(toRet).serialize() << std::endl;
            return toRet;
        }
}

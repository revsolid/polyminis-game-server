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
                float toExtract = JsonHelpers::json_get_float(request, "ExtractedBiomass");
                int   pid = JsonHelpers::json_get_int(request, "PlanetId");
                int   epoch = JsonHelpers::json_get_int(request, "Epoch");
                float percentageChange = BiomassToPopulationPercentage(toExtract);

                std::string speciesName = JsonHelpers::json_get_string(request, "SpeciesName");

                // Update 
                try
                {

                    std::string url = "/persistence/species_summaries/"+std::to_string(pid)+""+std::to_string(epoch)+"/"+speciesName+"/";
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
                        return picojson::object();
                    }

                    xtractPayload["Percentage"] = picojson::value(oldPercentage - percentageChange);

                    PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                         PolyminisServer::HttpMethod::PUT, xtractPayload);
                    sd.BiomassAvailable += toExtract; 

                    GameUtils::SaveBiomassValue(sd, mAlmanacServerCfg); 
                }
                catch (websocketpp::exception const & e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
            else if (command == "RESEARCH") 
            {
                // TO RESEARCH: Add an inventory entry with a Research entry (PID+EPOCH, NAME) and a Timestamp
            }
            else if (command == "SAMPLE") 
            {
                // TO SAMPLE: Add an inventory entry with the translation table of the species and a pointer (PID+EPOCH, NAME) to it
            }
            else if (command == "DEPLOY") 
            {
                // TO DEPLOY: Insert the species in the planet in epoch + 1 with the proportion based on the invested Biomass.
                // Copy the species in epoch to epoch+1 with the new proportions.
            }

            return toRet;
        }
}

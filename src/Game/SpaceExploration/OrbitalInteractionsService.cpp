#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "Core/Server.h"
#include "Game/GameUtils.h"

#include "OrbitalInteractionsService.h"

namespace SpaceExploration
{

        OrbitalInteractionsService::OrbitalInteractionsService(PolyminisServer::WSServer& server,
                                                               PolyminisServer::ServerCfg& almanacServerCfg,
                                                               PolyminisGameRules::GameRules& gameRules,
                                                               PlanetManager& pManager) :
                                                               mAlmanacServerCfg(almanacServerCfg),
                                                               mGameRules(gameRules),
                                                               mPlanetManager(pManager)
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
                float percentageChange = JsonHelpers::json_get_float(request, "ExtractedPercentage");
                float toExtract = mGameRules.GetPercentageToBiomass(percentageChange);

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
                    float newPercentage = fmax(0.0, oldPercentage - percentageChange);
                    xtractPayload["Percentage"] = picojson::value(newPercentage);
                    PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                         PolyminisServer::HttpMethod::PUT, xtractPayload);

                    picojson::object epochStatistics = GameDBUtils::GetEpochStatistics(mAlmanacServerCfg, pid, epoch);
                    picojson::object percObj = JsonHelpers::json_get_object(picojson::value(epochStatistics), "Percentages");
                    percObj[speciesName] = picojson::value((float)newPercentage);
                    epochStatistics["Percentages"] = picojson::value(percObj);
                    epochStatistics["PlanetId"] = picojson::value((float)pid);
                    epochStatistics["EpochNum"] = picojson::value((float)epoch);
                    GameDBUtils::UpdateEpochStatistics(mAlmanacServerCfg, epochStatistics, pid, epoch);

                    if (newPercentage < 0.01)
                    {
                        // Remove Species from planet
                        PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                             PolyminisServer::HttpMethod::DELETE, picojson::object());
                        
                        //
                        // TODO: Record Extinction Event anything else?
                    }

                    sd.BiomassAvailable += toExtract;

                    GameDBUtils::SaveBiomassValue(sd, mAlmanacServerCfg); 
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
            else if (command == "GET_TO_EDIT_IN_PLANET")
            {
                auto speciesJson = JsonHelpers::json_get_object(request, "Species");

                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");
                toRet["Species"] = picojson::value(GameDBUtils::GetSpeciesInPlanet(mAlmanacServerCfg, planetEpoch, speciesName));
                toRet["InPlanet"] = picojson::value((float)pid);

                toRet["EventString"] = picojson::value("GET_EDIT_RESULT");
            }
            else if (command == "EDIT_IN_PLANET")
            {
                auto speciesJson = JsonHelpers::json_get_object(request, "Species");
                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");
                picojson::object newSpeciesPayload;
                auto splices = picojson::value(JsonHelpers::json_get_array(picojson::value(speciesJson), "Splices"));
                newSpeciesPayload["Splices"] = splices;
                newSpeciesPayload["InstinctTuning"] = picojson::value(JsonHelpers::json_get_object(picojson::value(speciesJson), "InstinctTuning"));
                newSpeciesPayload["TranslationTable"] = mGameRules.CreateTranslationTable(splices);
                std::string url = "/persistence/speciessummaries/"+planetEpoch+"/"+speciesName;
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
                float percentageForDeployed = mGameRules.GetBiomassToPercentage(toDeploy);

                auto speciesJson = JsonHelpers::json_get_object(request, "Species");
                std::string speciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "SpeciesName");

                try
                {
                    if (toDeploy > sd.BiomassAvailable)
                    {
                        // Error
                        return JsonHelpers::json_create_error("Error - Trying to Deploy more Biomass than Available");
                    }

                    auto species_arr = GameDBUtils::GetAllSpeciesInPlanet(mAlmanacServerCfg, planetEpoch);

                    picojson::object epochStatistics = GameDBUtils::GetEpochStatistics(mAlmanacServerCfg, pid, epoch);
                    picojson::object percObj = JsonHelpers::json_get_object(picojson::value(epochStatistics), "Percentages");

                    float currentTotalPercentage = 0.0;
                    for (auto species_json : species_arr)
                    {
                        currentTotalPercentage += JsonHelpers::json_get_float(species_json, "Percentage");
                    }
                    for (auto species_json : species_arr)
                    {
                        float oldPercentage = JsonHelpers::json_get_float(species_json, "Percentage");
                        float newPercentage = oldPercentage -  (oldPercentage * percentageForDeployed);
                        if (currentTotalPercentage + percentageForDeployed < 1.0)
                        {
                            // Don't change percentages unless the planet is full
                            newPercentage = oldPercentage;
                        }
                        std::cout << "Percentage Set for Planet " << planetEpoch << " Old Percentage: " << oldPercentage;
                        std::cout  << " New Percentage: "<< newPercentage << std::endl;
                        std::cout  << "Deploying: " << percentageForDeployed << std::endl;
                        std::string sName = JsonHelpers::json_get_string(species_json, "SpeciesName");

                        picojson::object xtractPayload;
                        xtractPayload["Percentage"] = picojson::value((float)newPercentage);
                        std::string url = "/persistence/speciessummaries/"+planetEpoch+"/"+sName;
                        PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, url,
                                                             PolyminisServer::HttpMethod::PUT, xtractPayload);
                        percObj[sName] = picojson::value((float)newPercentage);
                    }
                    percObj[speciesName] = picojson::value(percentageForDeployed);

                    epochStatistics["Percentages"] = picojson::value(percObj);
                    epochStatistics["PlanetId"] = picojson::value((float)pid);
                    epochStatistics["EpochNum"] = picojson::value((float)epoch);
                    GameDBUtils::UpdateEpochStatistics(mAlmanacServerCfg, epochStatistics, pid, epoch);

                    picojson::array individuals;
                    picojson::object config;
                    {
                        auto originalSpeciesName = JsonHelpers::json_get_string(picojson::value(speciesJson), "OriginalSpeciesName");
                        auto originalPlanetEpoch = JsonHelpers::json_get_string(picojson::value(speciesJson), "PlanetEpoch");
                        auto genPayload = GameDBUtils::GetSpeciesGeneticPayload(mAlmanacServerCfg, originalPlanetEpoch, originalSpeciesName);  

                        config = JsonHelpers::json_get_object(picojson::value(genPayload), "GAConfiguration");
                        individuals = JsonHelpers::json_get_array(picojson::value(genPayload), "Individuals");
                    }

                    sd.BiomassAvailable -= toDeploy;
                    GameDBUtils::SaveBiomassValue(sd, mAlmanacServerCfg);
                    toRet["EventString"] = picojson::value("DEPLOY_RESULT");
                    toRet["NewBiomassAvailable"] = picojson::value(sd.BiomassAvailable);

                    picojson::object newSpeciesPayload;
                    newSpeciesPayload["Percentage"] = picojson::value(percentageForDeployed);
                    newSpeciesPayload["PlanetEpoch"] = picojson::value(planetEpoch);
                    newSpeciesPayload["SpeciesName"] = picojson::value(speciesName);
                    newSpeciesPayload["CreatorName"] = picojson::value(sd.UserName);
                    auto splices = picojson::value(JsonHelpers::json_get_array(picojson::value(speciesJson), "Splices"));
                    newSpeciesPayload["Splices"] = splices;
                    auto instinctTuning = picojson::value(JsonHelpers::json_get_object(picojson::value(speciesJson), "InstinctTuning"));
                    newSpeciesPayload["InstinctTuning"] = instinctTuning;
                    newSpeciesPayload["TranslationTable"] = mGameRules.CreateTranslationTable(splices);
                    newSpeciesPayload["GAConfiguration"] = picojson::value(config);
                    newSpeciesPayload["Individuals"] = picojson::value(individuals);

                    newSpeciesPayload["InstinctWeights"] = mGameRules.CreateInstinctWeights(instinctTuning);

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
                picojson::array species = GameDBUtils::GetAllSpeciesInPlanet(mAlmanacServerCfg, planetEpoch);
                planet.SwapSpecies(species);

                toRet["EventString"] = picojson::value("INTERACTION_RESULT");
                toRet["UpdatedPlanet"] = picojson::value(planet.ToJson());
                std::cout << "Reloading Planet... y'all" << std::endl;
            }

            std::cout << " Sending down Event: " << picojson::value(toRet).serialize() << std::endl;
            return toRet;
        }
}

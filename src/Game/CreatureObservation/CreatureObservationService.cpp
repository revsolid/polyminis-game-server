#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "Game/GameUtils.h"
#include "CreatureObservationService.h"

#include <thread>

namespace CreatureObservation
{
    CreatureObservationService::CreatureObservationService(PolyminisServer::WSServer& server,
                                                           SimulationServerConfig& simulationServerCfg,
                                                           SimulationServerConfig& almanacServerCfg,
                                                           PolyminisGameRules::GameRules& gameRules) :
                                                           mSimServerCfg(simulationServerCfg),
                                                           mAlmanacServerCfg(almanacServerCfg),
                                                           mGameRules(gameRules)
    {
    
        try
        {
            std::cout << "Requesting Simulation Server Status..." << std::endl;
            picojson::object simServerStatus = PolyminisServer::HttpClient::Request(mSimServerCfg.host, mSimServerCfg.port,
                                                                                    "/", PolyminisServer::HttpMethod::GET,
                                                                                    picojson::object());
            std::cout << "Done..." << std::endl;
        }
        catch (websocketpp::exception const & e) 
        {
            std::cout << e.what() << std::endl;
        }
        

        // TODO: Check the server status and if it isn't up, do something about it, error out (?)


        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "creature_observation";
        wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sd)
                         {
                             return this->CreatureObservationEndpoint(request, sd);
                         };
        server.AddService(wss);
    }

    CreatureObservationService::~CreatureObservationService()
    {
    }

    picojson::object CreatureObservationService::CreatureObservationEndpoint(picojson::value& request, PolyminisServer::SessionData& sd)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_as_object(request);

        picojson::object toRet;
        toRet["Service"] = picojson::value("creature_observation");

        // Create a new simulation server for this session or use whatever is in the Session
        if (sd.SimulationServerId == SIM_SERVER_ID_INVALID)
        {
            // Request a new server
            auto res = GameSimUtils::CreateSimulationServer(mSimServerCfg);

            if (res.count("SimulationId") > 0)
            {
                sd.SimulationServerId = JsonHelpers::json_get_as_float(res["SimulationId"]);
            }
            else
            {
                std::cout << request.serialize() << std::endl;
            }
            
            if (sd.SimulationServerId == SIM_SERVER_ID_INVALID)
            {
                return JsonHelpers::json_create_error("Could not create Simulation Server");
            }
            else
            {
                SimulationConnection s(sd.SimulationServerId);
                mConnections[sd.SimulationServerId] = std::move(s);
            }

        }
    
        // Change the Epoch of Simulation Server
        if (command == "GO_TO_EPOCH")
        {
            int pid   = 3;
            int epoch = 16; 
            int step = 0;

            pid = JsonHelpers::json_get_int(request, "PlanetId");
            epoch = JsonHelpers::json_get_int(request, "Epoch");

            GameSimUtils::RunSimulation(mSimServerCfg, mAlmanacServerCfg, mGameRules.GetTraitData(), sd.SimulationServerId, pid, epoch);
            auto& simConnection = mConnections[sd.SimulationServerId];
            {
                simConnection.Epoch = epoch;
                simConnection.Step = step;
            }

            toRet["EventString"] = picojson::value("NEW_EPOCH");
            toRet["Species"] = picojson::value(GameSimUtils::GetSpecies(mSimServerCfg, sd.SimulationServerId, epoch));
            toRet["Environment"] =  picojson::value(GameSimUtils::GetEnvironment(mSimServerCfg, sd.SimulationServerId, epoch));
        }
        else if (command == "POLL")
        {
            picojson::array toSend;
            auto& simConnection = mConnections[sd.SimulationServerId];
            {
                int step = simConnection.Step;
                int epoch = simConnection.Epoch;

                {
                    auto res = GameSimUtils::GetSimulationSteps(mSimServerCfg, sd.SimulationServerId, epoch, step, 10); 
                    for(auto r : res)
                    {
                        toSend.push_back(r);
                    }
                }

                simConnection.Step = step;
            }

            if (toSend.size() == 0)
            {
                toRet["EventString"] = picojson::value("SIM_DONE");
            }
            else
            {
                toRet["EventString"] = picojson::value("STEP_POLL");
                toRet["Steps"] = picojson::value(toSend);
            }
        }
        else if (command == "GET_STATS")
        {
            int pid = JsonHelpers::json_get_int(request, "PlanetId");
            int epoch = JsonHelpers::json_get_int(request, "Epoch");

            /* Break EpochStats into something Unity Serializer can consume */
            auto epochStats = GameDBUtils::GetEpochStatistics(mAlmanacServerCfg, pid, epoch);

            auto epochStatsAsArrays = picojson::object();

            for (auto it = epochStats.begin(); it != epochStats.end(); it++)
            {
                if (it->first == "PlanetId")
                {
                    continue;
                }
                                
                if (it->first == "EpochNum")
                {
                    epochStatsAsArrays["Epoch"] = it->second;
                    continue;
                }

                auto stat = JsonHelpers::json_get_as_object(it->second);
                auto statAsArray = picojson::array();
                for (auto it_prime = stat.begin(); it_prime != stat.end(); it_prime++)
                {
                    auto entry = picojson::object(); 
                    entry["SpeciesName"] = picojson::value(it_prime->first);
                    entry["Value"] = picojson::value(JsonHelpers::json_get_as_float(it_prime->second));
                    statAsArray.push_back(picojson::value(entry));
                }
                epochStatsAsArrays[it->first] = picojson::value(statAsArray);
            }

            toRet["EpochStats"] = picojson::value(epochStatsAsArrays);
            toRet["EventString"] = picojson::value("EPOCH_STATS");
        }

        std::cout << " Returning Creature Obs: " << std::endl;
        return std::move(toRet);
    }
}

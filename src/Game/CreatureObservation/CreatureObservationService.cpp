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

/*
        mSimulationPollingSlave = std::thread(
            [=]
            {
                //
                while (!mStopPolling)
                {
                    for (auto& kv : mConnections) 
                    {
                        int session = kv.first;

                        {
                            std::lock_guard<std::mutex>(*kv.second.Lock);
                            if (!kv.second.Poll)
                            {
                                continue;
                            }
                        }

                        int step = -1;
                        int epoch = -1;
                        {
                            std::lock_guard<std::mutex>(*kv.second.Lock);
                            step = kv.second.Step;
                            epoch = kv.second.Epoch;
                        }

                        // Go Fetch information from the Sim Server
                        {
                            auto res = GameSimUtils::GetSimulationSteps(mSimServerCfg, session, epoch, step); 

                            {
                                std::lock_guard<std::mutex>(*kv.second.Lock);
                                // Write the information into the StepsToSend field
                                for(auto r : res)
                                {
                                    kv.second.StepsToSend.push(r);
                                }
                                
                                // Update the State
                                kv.second.Step = step; 
                                kv.second.Epoch = epoch; // (? - Not sure if epoch changing here makes any sense) 
                            }
                        }
                    }
                    //
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            }
        );
*/
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

            std::this_thread::sleep_for(std::chrono::seconds(2));

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
                    auto res = GameSimUtils::GetSimulationSteps(mSimServerCfg, sd.SimulationServerId, epoch, step, 50); 
                    for(auto r : res)
                    {
                        toSend.push_back(r);
                    }
                }

                simConnection.Step = step;
            }
            toRet["EventString"] = picojson::value("STEP_POLL");
            toRet["Steps"] = picojson::value(toSend);
        }

        std::cout << " Returning Creature Obs: " << std::endl;
        return std::move(toRet);
    }
}

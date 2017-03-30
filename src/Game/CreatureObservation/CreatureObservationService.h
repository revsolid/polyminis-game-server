#pragma once
#include "Core/Server.h"
#include "Game/GameRules.h"
#include <map>
#include <memory>
#include <queue>

namespace CreatureObservation
{
    typedef PolyminisServer::ServerCfg SimulationServerConfig;
    typedef int SimulationStep;


    struct SimulationConnection
    {
        std::shared_ptr<std::mutex> Lock;

        uint Epoch;
        uint Step;
        std::queue<picojson::value> StepsToSend; 

        bool Poll = false;

        SimulationConnection() : Epoch(0), Step(0), Poll(false)
        {
            Lock = std::make_shared<std::mutex>();
        }

        SimulationConnection(uint e) : Epoch(e), Step(0), Poll(false)
        {
            Lock = std::make_shared<std::mutex>();
        }
    };

    class CreatureObservationService
    {
    public:
        CreatureObservationService(PolyminisServer::WSServer& server,
                                   SimulationServerConfig& simulationServerCfg,
                                   SimulationServerConfig& almanacServerCfg,
                                   PolyminisGameRules::GameRules& gameRules);
        ~CreatureObservationService();
        picojson::object CreatureObservationEndpoint(picojson::value& request, PolyminisServer::SessionData& sd);
    private:
        // Members
        SimulationServerConfig& mSimServerCfg; 
        SimulationServerConfig& mAlmanacServerCfg; 
        PolyminisGameRules::GameRules& mGameRules;

        // TODO: Useful comment about how this slave works
        bool mStopPolling = false;
        std::unordered_map<int, SimulationConnection> mConnections;
        std::thread mSimulationPollingSlave;
    };
}

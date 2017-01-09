#pragma once
#include "Core/Server.h"

namespace CreatureObservation
{
    struct SimulationServerConfig
    {
        std::string host;
        int port;
    };

    class CreatureObservationService
    {
    public:
        CreatureObservationService(PolyminisServer::WSServer& server,
                                   SimulationServerConfig& simulationServerCfg);
        picojson::object CreatureObservationEndpoint(picojson::value& command);
    private:
        // Members
        SimulationServerConfig& mSimServerCfg; 
    };
}

#pragma once
#include "Core/Server.h"

namespace CreatureObservation
{
    typedef PolyminisServer::ServerCfg SimulationServerConfig;

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

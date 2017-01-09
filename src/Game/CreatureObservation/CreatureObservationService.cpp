#include "CreatureObservationService.h"

namespace CreatureObservation
{
    CreatureObservationService::CreatureObservationService(PolyminisServer::WSServer& server,
                                                           SimulationServerConfig& simulationServerCfg) :
                                                           mSimServerCfg(simulationServerCfg) 
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "space_exploration";
        wss->mHandler =  [=] (picojson::value& request)
                         {
                             return this->CreatureObservationEndpoint(request);
                         };
        server.AddService(wss);
    }

    picojson::object CreatureObservationService::CreatureObservationEndpoint(picojson::value& command)
    {
        picojson::object to_ret;

	// Create a new simulation server for this session
	

        // Change the Epoch of Simulation Server

        // Change the Environment of Simulation Server


        return std::move(to_ret);
    }
}

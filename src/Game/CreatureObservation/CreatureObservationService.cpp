#include "CreatureObservationService.h"
#include "Core/HttpClient.h"

namespace CreatureObservation
{
    CreatureObservationService::CreatureObservationService(PolyminisServer::WSServer& server,
                                                           SimulationServerConfig& simulationServerCfg) :
                                                           mSimServerCfg(simulationServerCfg) 
    {
	
	picojson::object simServerStatus = PolyminisServer::HttpClient::Request(mSimServerCfg.host, mSimServerCfg.port,
			                                                        "/simulations", PolyminisServer::HttpMethod::GET,
					                                        picojson::object());

	// TODO: Check the server status and if it isn't up, do something about it, error out (?)


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

#include <iostream>
#include <picojson.h>
#include "Main.h"
#include "Core/Server.h"
#include "Game/SpaceExploration/SpaceExplorationService.h"
#include "Game/SpaceExploration/PlanetManager.h"
#include "Game/SpaceExploration/SpaceMap.h"
#include "Game/CreatureObservation/CreatureObservationService.h"
#include "Game/User/UserService.h"

int main()
{
    try 
    {
        std::cout << "Creating Server..." << std::endl;
        PolyminisServer::WSServer server;
        std::cout << "Done.." << std::endl;
	    PolyminisServer::ServerCfg almanacServer { "localhost", 8081 };

        PlanetManager pManager({});

        std::cout << "Adding Space Exploration..." << std::endl;
        SpaceExploration::SpaceExplorationService spaceEx(server, pManager, almanacServer);

        std::cout << "Adding User Service..." << std::endl;
        User::UserService userService(server, almanacServer);

        CreatureObservation::SimulationServerConfig simServerCfg { std::string("localhost"), 8080 };
        std::cout << "Adding Creature Observation..." << std::endl;
        CreatureObservation::CreatureObservationService creatureObs(server, simServerCfg);

        std::cout << "Starting Server..." << std::endl;
        server.RunServer();
    }
    catch (websocketpp::exception const & e) 
    {
        std::cout << e.what() << std::endl;
    }
    catch (const std::exception  & e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

}

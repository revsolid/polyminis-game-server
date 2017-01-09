#include <iostream>
#include <picojson.h>
#include "Main.h"
#include "Core/Server.h"
#include "Game/SpaceExploration/SpaceExplorationService.h"
#include "Game/SpaceExploration/PlanetManager.h"
#include "Game/SpaceExploration/SpaceMap.h"
#include "Game/CreatureObservation/CreatureObservationService.h"

int main()
{
    try 
    {

	PolyminisServer::WSServer server;
        PlanetManager pManager({});

        int id = 0;
        for(float i = -500.0f; i <= 500.0f; i += 200)
        {
            for(float j = -500.0f; j <= 500.0f; j += 200)
            {
                pManager.AddPlanet(i, j, id++);
            }
        }
        SpaceExploration::SpaceExplorationService spaceEx(server, pManager);

        CreatureObservation::SimulationServerConfig simServerCfg { std::string("localhost"), 8080 };
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

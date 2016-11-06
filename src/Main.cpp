#pragma warning(disable:4996)

#include <iostream>
#include <picojson.h>
#include "Main.h"
#include "Core/Tools.h"
#include "Core/Server.h"
#include "Game/SpaceExploration/SpaceExplorationService.h"
#include "Game/SpaceExploration/PlanetManager.h"
#include "Game/SpaceExploration/SpaceMap.h"

int main()
{
    try 
    {
        PolyminisServer::WSServer server;

        PlanetManager pManager({});

        unsigned int id = 0;
        for(float i = -500.0f; i <= 500.0f; i += 200)
        {
            for(float j = -500.0f; j <= 500.0f; j += 200)
            {
                pManager.AddPlanet(i, j, id++);
            }
        }
        SpaceExploration::SpaceExplorationService spaceEx(server, pManager);
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

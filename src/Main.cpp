#include <iostream>
#include <picojson.h>
#include "Main.h"
#include "Core/Server.h"
#include "Game/GameRules.h"
#include "Game/SpaceExploration/SpaceExplorationService.h"
#include "Game/SpaceExploration/PlanetManager.h"
#include "Game/SpaceExploration/SpaceMap.h"
#include "Game/SpaceExploration/OrbitalInteractionsService.h"
#include "Game/CreatureObservation/CreatureObservationService.h"
#include "Game/Inventory/InventoryService.h"
#include "Game/User/UserService.h"

//TODO - Define this through CMake / CMD
#define DEVMODE

#ifdef DEVMODE
    #include "Game/Admin/AdminService.h"
#endif

int main()
{
    try 
    {
        std::cout << "Creating Server..." << std::endl;
        PolyminisServer::WSServer server;
        std::cout << "  Done." << std::endl;

	    PolyminisServer::ServerCfg almanacServer { "localhost", 8081 };

        std::cout << "Loading Server-Wide Game Rules..." << std::endl;
        PolyminisGameRules::GameRules gameRules(almanacServer); 
        std::cout << "  Done." << std::endl;

        PlanetManager pManager({});

        std::cout << "Adding Space Exploration..." << std::endl;
        SpaceExploration::SpaceExplorationService spaceEx(server, almanacServer, gameRules, pManager);

        std::cout << "Adding OrbitalInteractions Service..." << std::endl;
        SpaceExploration::OrbitalInteractionsService orbitalInteractions(server, almanacServer, gameRules, pManager);
     
        std::cout << "Adding User Service..." << std::endl;
        User::UserService userService(server, almanacServer);

        CreatureObservation::SimulationServerConfig simServerCfg { std::string("localhost"), 8082 };
        std::cout << "Adding Creature Observation..." << std::endl;
        CreatureObservation::CreatureObservationService creatureObs(server, simServerCfg, almanacServer, gameRules);

        std::cout << "Adding Inventory Service..." << std::endl;
        Inventory::InventoryService inventory(server, almanacServer, gameRules);

#ifdef DEVMODE
        // Make it super clear if it's devMode or not
        std::cout << "***********************************" << std::endl;
        std::cout << "***********************************" << std::endl;
        std::cout << "Adding Admin Service" << std::endl;
        std::cout << "***********************************" << std::endl;
        std::cout << "***********************************" << std::endl;

        Admin::AdminService adminService(server, almanacServer, gameRules);

        std::cout << "Done. " << std::endl;
#endif

        std::cout << "Starting Server..." << std::endl;
        server.RunServer();
    }
    catch (websocketpp::exception const &e) 
    {
        std::cout << e.what() << std::endl;
    }
    catch (const std::exception  &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

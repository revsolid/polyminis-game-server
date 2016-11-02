#pragma warning(disable:4996)

#include <iostream>
#include <picojson.h>
#include "Main.h"
#include "Core/Tools.h"
#include "Core/Server.h"
#include "Game/SpaceExploration/SpaceExplorationService.h"
#include "Game/SpaceExploration/PlanetManager.h"
#include "Game/SpaceExploration/SpaceMap.h"


/*
PlanetManager* pManager;
SpaceMapSession* ship;
float visibilityRange = 600.0f;

// send messageContent to client
void SendOutMessage(server* s, websocketpp::connection_hdl hdl, message_ptr msgptr, std::string messageContent)
{
    try
    {
        s->send(hdl, messageContent, msgptr->get_opcode());
    }
    catch (const websocketpp::lib::error_code& e)
    {
        std::cout << "Send Message Failed: " << e << "(" << e.message() << ")" << std::endl;
    }
}

// take location of ship variable and 
// send all the planets visible from there.
void SendPlanetsData(server* s, websocketpp::connection_hdl hdl, message_ptr msg)
{
    std::vector<std::string> spawnMsgs = pManager->SpawnVisibleStrings(ship->GetPos(), visibilityRange);
    if (!spawnMsgs.empty())
    {
        for (auto i : spawnMsgs)
        {
            SendOutMessage(s, hdl, msg, i);
        }
    }
}

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) 
{
    //std::cout << "on_message called with hdl: " << hdl.lock().get() << " and message: " << msg->get_payload() << std::endl;

    std::string message = msg->get_payload();
    std::string messageType = message.substr(1, message.find(">[") - 1);
    std::string messageLoad = message.substr(message.find(">[") + 2, message.length() - messageType.length() - 4);

    std::cout << "Message received: " << message << std::endl;
    
    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") 
    {
        s->stop_listening();
        return;
    }
    else if (messageType == "mov") // attempt movement
    {

        Coord newCoord = Tools::Parse2DCoord(messageLoad);
        bool approved = ship->AttemptMove(newCoord);
        if (!approved)
        {
            std::string denyStr = "<kickback>[";
            denyStr += std::to_string(ship->GetPos().x);
            denyStr += ",";
            denyStr += std::to_string(ship->GetPos().y);
            denyStr += "]";

            SendOutMessage(s, hdl, msg, denyStr);
        }
        SendPlanetsData(s, hdl, msg);
    }
    else if (messageType == "init")
    {
        ship = new SpaceMapSession(Tools::Parse2DCoord(messageLoad));

        SendPlanetsData(s, hdl, msg);
    }
}
 */

void InitPlanets()
{
    auto pManager = new PlanetManager({
        { 10.0f, 10.0f },
        { 30.0f, 10.0f },
        { 50.0f, 10.0f },
        { -10.0f, 10.0f },
        { -30.0f, 10.0f },
        { -50.0f, 10.0f },

        { 10.0f, 30.0f },
        { 30.0f, 30.0f },
        { 50.0f, 30.0f },
        { -10.0f, 30.0f },
        { -30.0f, 30.0f },
        { -50.0f, 30.0f },

        { 10.0f, 50.0f },
        { 30.0f, 50.0f },
        { 50.0f, 50.0f },
        { -10.0f, 50.0f },
        { -30.0f, 50.0f },
        { -50.0f, 50.0f }
        /*
        { 100.0f, -100.0f },
        { 300.0f, -100.0f },
        { 500.0f, -100.0f },
        { -100.0f, -100.0f },
        { -300.0f, -100.0f },
        { -500.0f, -100.0f },

        { 100.0f, -300.0f },
        { 300.0f, -300.0f },
        { 500.0f, -300.0f },
        { -100.0f, -300.0f },
        { -300.0f, -300.0f },
        { -500.0f, -300.0f },

        { 100.0f, -500.0f },
        { 300.0f, -500.0f },
        { 500.0f, -500.0f },
        { -100.0f, -500.0f },
        { -300.0f, -500.0f },
        { -500.0f, -500.0f }
        */
    });
}


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
                pManager.AddPlanet(i, j, i++);
            }
        }
        SpaceExplorationService spaceEx(server, pManager);
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

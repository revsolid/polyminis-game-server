#pragma warning(disable:4996)

#include <iostream>
#include "Main.h"
#include "Game/SpaceExploration/PlanetManager.h"
//#include "Core/OS.h"
//#include "Core/Types.h"
//#include "Game/SpaceExploration/SpaceMap.h"



typedef websocketpp::server<websocketpp::config::asio> server;


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;


PlanetManager* pManager;
float visibilityRange = 3.0f;

float playerPosX = 0;
float playerPosY = 0;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
	//std::cout << "on_message called with hdl: " << hdl.lock().get() << " and message: " << msg->get_payload() << std::endl;

	std::string message = msg->get_payload();
	std::string messageType = message.substr(1, message.find(">[") - 1);
	std::string messageLoad = message.substr(message.find(">[") + 2, message.length() - messageType.length() - 4);

	std::cout << "Message received: " << message << std::endl;
	
	// check for a special command to instruct the server to stop listening so
	// it can be cleanly exited.
	if (msg->get_payload() == "stop-listening") {
		s->stop_listening();
		return;
	}
	else if (messageType == "loc") // Location update, send planet to spawn and despawn
	{
		std::string xString = messageLoad.substr(0, messageLoad.find(",") - 1);
		std::string yString = messageLoad.substr(messageLoad.find(",") + 1, messageLoad.length() - xString.length() - 1);
		playerPosX = atof(xString.c_str());
		playerPosY = atof(yString.c_str());
		std::cout << playerPosX << ": " << playerPosY << std::endl;
		pManager->UpdatePlanetsVisibility(playerPosX, playerPosY, visibilityRange);
		std::vector<std::string> spawnMsgs = pManager->SpawnStrings();

		if (!spawnMsgs.empty())
		{
			for (auto i : spawnMsgs)
			{
				try
				{
					s->send(hdl, i, msg->get_opcode());
					std::cout << "Spawn message:" << i << std::endl;
				}
				catch (const websocketpp::lib::error_code& e)
				{
					std::cout << "SpawningMsg Failed: " << e << "(" << e.message() << ")" << std::endl;
				}
			}
		}
	}	
}

void InitPlanets()
{
	pManager = new PlanetManager({
		{ 1.0f, 1.0f },
		{ 3.0f, 1.0f },
		{ 5.0f, 1.0f },
		{ -1.0f, 1.0f },
		{ -3.0f, 1.0f },
		{ -5.0f, 1.0f },

		{ 1.0f, 3.0f },
		{ 3.0f, 3.0f },
		{ 5.0f, 3.0f },
		{ -1.0f, 3.0f },
		{ -3.0f, 3.0f },
		{ -5.0f, 3.0f },

		{ 1.0f, 5.0f },
		{ 3.0f, 5.0f },
		{ 5.0f, 5.0f },
		{ -1.0f, 5.0f },
		{ -3.0f, 5.0f },
		{ -5.0f, 5.0f },

		{ 1.0f, -1.0f },
		{ 3.0f, -1.0f },
		{ 5.0f, -1.0f },
		{ -1.0f, -1.0f },
		{ -3.0f, -1.0f },
		{ -5.0f, -1.0f },

		{ 1.0f, -3.0f },
		{ 3.0f, -3.0f },
		{ 5.0f, -3.0f },
		{ -1.0f, -3.0f },
		{ -3.0f, -3.0f },
		{ -5.0f, -3.0f },

		{ 1.0f, -5.0f },
		{ 3.0f, -5.0f },
		{ 5.0f, -5.0f },
		{ -1.0f, -5.0f },
		{ -3.0f, -5.0f },
		{ -5.0f, -5.0f }
	});

	pManager->UpdatePlanetsVisibility(playerPosX, playerPosY, visibilityRange);
}

// check if any planets need to spawn or despawn,
// and send the spawn/despawn massages
void UpdatePlanets()
{
	pManager->UpdatePlanetsVisibility(playerPosX, playerPosY, visibilityRange);
	
}

void UpdatePlayerPos(float x, float y)
{
	playerPosX = x;
	playerPosY = y;
}


int main() 
{

	// Create a server endpoint
	server game_server;
	InitPlanets();

	try 
	{
		// Set logging settings
		game_server.set_access_channels(websocketpp::log::alevel::all);
		game_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize Asio
		game_server.init_asio();

		// Register our message handler
		game_server.set_message_handler(bind(&on_message, &game_server, ::_1, ::_2));

		// Listen on port 9002
		game_server.listen(9002);

		// Start the server accept loop
		game_server.start_accept();

		// Start the ASIO io_service run loop
		game_server.run();
	}
	catch (websocketpp::exception const & e) 
	{
		std::cout << e.what() << std::endl;
	}
	catch (...) 
	{
		std::cout << "other exception" << std::endl;
	}



}

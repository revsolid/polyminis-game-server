#pragma warning(disable:4996)

#include <iostream>
#include "Main.h"
#include "Core\Tools.h"
#include "Game/SpaceExploration/PlanetManager.h"
#include "Game/SpaceExploration/SpaceMap.h"
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
SpaceMapSession* ship;
float visibilityRange = 3.0f;

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
}


int main() {
	// Create a server endpoint
	server echo_server;
	InitPlanets();

	try {

		// Set logging settings
		echo_server.set_access_channels(websocketpp::log::alevel::all);
		echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize Asio
		echo_server.init_asio();

		// Register our message handler
		echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));

		// Listen on port 9002
		echo_server.listen(8080);

		// Start the server accept loop
		echo_server.start_accept();

		// Start the ASIO io_service run loop
		echo_server.run();
	}
	catch (websocketpp::exception const & e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "other exception" << std::endl;
	}

}
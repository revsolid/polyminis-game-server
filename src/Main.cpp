#include "Main.h"
#include "Core/OS.h"
#include "Core/Types.h"
#include "Game/SpaceExploration/SpaceMap.h"

class Server
{
public:
    void onStartGameSession(Aws::GameLift::Server::Model::GameSession myGameSession);
    void onProcessTerminate();
    bool onHealthCheck();
    void Start();
};
void Server::onStartGameSession(Aws::GameLift::Server::Model::GameSession myGameSession)
{
   // game-specific tasks when starting a new game session, such as loading map
   Aws::GameLift::GenericOutcome outcome = 
             Aws::GameLift::Server::ActivateGameSession();
}

void Server::onProcessTerminate()
{
  // game-specific tasks required to gracefully shut down a game session, 
  // such as notifying players, preserving game state data, and other cleanup
  Aws::GameLift::GenericOutcome outcome = Aws::GameLift::Server::ProcessEnding();
}

bool Server::onHealthCheck()
{
  bool health = true;
  // complete health evaluation within 60 seconds and set health
  return health;
}

void Server::Start()
{
// Set parameters and call ProcessReady
// Example of a log file
   std::string serverLog("serverOut.log");
   std::vector<std::string> logPaths;
   logPaths.push_back(serverLog);
   Aws::GameLift::Server::InitSDK();
   Aws::GameLift::Server::ProcessParameters processReadyParameter = Aws::GameLift::Server::ProcessParameters(
       std::bind(&Server::onStartGameSession, this, std::placeholders::_1),
       std::bind(&Server::onProcessTerminate, this),
       std::bind(&Server::onHealthCheck, this),
       55666, logPaths );
   Aws::GameLift::Server::ProcessReady(processReadyParameter);
}

int main()
{

  Server server;
  server.Start();
}

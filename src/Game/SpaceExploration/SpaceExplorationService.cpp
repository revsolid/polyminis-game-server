#include "SpaceExplorationService.h"


SpaceExplorationService::SpaceExplorationService(PolyminisServer::WSServer& server,
                                                 PlanetManager& pManager) :
                                                 mPlanetManager(pManager)
{
    auto wss = std::make_shared<PolyminisServer::WSService>();
    wss->mServiceName = "control";
    wss->mHandler =  [=] (picojson::value& request)
                     {
                         return this->SpaceExplorationEndpoint(request);
                     };
    server.AddService(wss);
}

std::string SpaceExplorationService::SpaceExplorationEndpoint(picojson::value& command)
{

}

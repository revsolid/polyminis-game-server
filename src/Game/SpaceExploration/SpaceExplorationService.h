#pragma once
#include "Core/Server.h"
#include "PlanetManager.h"
#include "SpaceMap.h"

class SpaceExplorationService
{
public:
    SpaceExplorationService(PolyminisServer::WSServer& server,
                            PlanetManager& pManager);
    std::string SpaceExplorationEndpoint(picojson::value& command);
private:
    PlanetManager& mPlanetManager;
    SpaceMapSession* mSpaceMapSession; 
};

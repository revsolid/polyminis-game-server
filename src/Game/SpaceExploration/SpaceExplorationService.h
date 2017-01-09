#pragma once
#include "Core/Server.h"
#include "PlanetManager.h"
#include "SpaceMap.h"

namespace SpaceExploration
{
    class SpaceExplorationService
    {
    public:
        SpaceExplorationService(PolyminisServer::WSServer& server,
                                PlanetManager& pManager);
        picojson::object SpaceExplorationEndpoint(picojson::value& command);
    private:
        picojson::object CreatePlanetaryPayload();
        picojson::object CreateWarpPayload(Coord dest);

        // Members
        PlanetManager& mPlanetManager;
        SpaceMapSession mSpaceMapSession; 
    };
}

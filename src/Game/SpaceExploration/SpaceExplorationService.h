#pragma once
#include "Core/Server.h"
#include "Game/GameRules.h"
#include "PlanetManager.h"
#include "SpaceMap.h"

namespace SpaceExploration
{
    class SpaceExplorationService
    {
    public:
        SpaceExplorationService(PolyminisServer::WSServer& server,
                                PolyminisServer::ServerCfg& almanacServerCfg,
                                PolyminisGameRules::GameRules& gameRules,
                                PlanetManager& pManager);
        picojson::object SpaceExplorationEndpoint(picojson::value& request, PolyminisServer::SessionData& sd);
    private:
        void CreatePlanetaryPayload(picojson::object& planetsSpawnEvent, const Coord& pos, float visRange);
        void CreateWarpPayload(picojson::object& warpToPlanetEvent, const Coord& dest);
        bool SavePositionOnDB(const Coord& pos, const std::string& userName);

        float CalcWarpCost(const Coord& start, const Coord& target);

        // Members
        PlanetManager& mPlanetManager;
        PolyminisServer::ServerCfg& mAlmanacServerCfg;
        PolyminisGameRules::GameRules& mGameRules;
    };
}

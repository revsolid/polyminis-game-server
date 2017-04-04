#pragma once
#include "Core/Server.h"
#include "Game/GameRules.h"
#include "PlanetManager.h"
#include "SpaceMap.h"


namespace SpaceExploration
{
    class OrbitalInteractionsService 
    {
    public:
        OrbitalInteractionsService(PolyminisServer::WSServer& server,
                                   PolyminisServer::ServerCfg& almanacServerCfg,
                                   PolyminisGameRules::GameRules& gameRules,
                                   PlanetManager& pManager);
                                   

        picojson::object OISEndpoint(picojson::value& request, PolyminisServer::SessionData& sd);

    private:
        PolyminisServer::ServerCfg mAlmanacServerCfg;
        PlanetManager& mPlanetManager;
        PolyminisGameRules::GameRules& mGameRules;
    };
}

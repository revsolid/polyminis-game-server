#pragma once
#include "Core/Server.h"
#include "Game/GameRules.h"

namespace Admin 
{
    class AdminService 
    {
    public:
        AdminService(PolyminisServer::WSServer& server, 
                     PolyminisServer::ServerCfg& almanacServerCfg,   
                     PolyminisGameRules::GameRules& gameRules);
        picojson::object AdminEndpoint(picojson::value& command, PolyminisServer::SessionData& sd);
    private:
        PolyminisGameRules::GameRules& mGameRules;
        PolyminisServer::ServerCfg& mAlmanacServerCfg;
    };
}

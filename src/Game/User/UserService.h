#pragma once
#include "Core/Server.h"

namespace User 
{
    class UserService
    {
    public:
        UserService(PolyminisServer::WSServer& server,
                    PolyminisServer::ServerCfg almanacServerCfg);
        picojson::object UserEndpoint(picojson::value& command, PolyminisServer::SessionData& sessionData);
    private:
        // Members
        PolyminisServer::ServerCfg mAlmanacServerCfg;
    };
}

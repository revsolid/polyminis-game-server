#pragma once
#include "Core/Server.h"

namespace User 
{
    class UserService
    {
    public:
        UserService(PolyminisServer::WSServer& server);
        picojson::object UserEndpoint(picojson::value& command);
    private:
        // Members
    };
}

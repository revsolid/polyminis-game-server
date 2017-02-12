#include "UserService.h"
namespace User
{
    UserService::UserService(PolyminisServer::WSServer& server)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "user";
        wss->mHandler =  [=] (picojson::value& request)
                         {
                             return this->UserEndpoint(request);
                         };
        server.AddService(wss);
    }

    picojson::object UserService::UserEndpoint(picojson::value& command)
    {
        picojson::object to_ret;
        return std::move(to_ret);
    }
}

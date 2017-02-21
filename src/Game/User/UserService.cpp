#include "UserService.h"
#include "Core/HttpClient.h"
#include "Core/JsonHelpers.h"

namespace User
{
    UserService::UserService(PolyminisServer::WSServer& server, 
                             PolyminisServer::ServerCfg almanacServerCfg) :
                             mAlmanacServerCfg(almanacServerCfg)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "user";
        wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sessionData)
                         {
                             return this->UserEndpoint(request, sessionData);
                         };
        server.AddService(wss);
    }

    picojson::object UserService::UserEndpoint(picojson::value& request, PolyminisServer::SessionData& sessionData)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_as_object(request);

        if (payload.count("UserName") == 0)
        {  
            return JsonHelpers::json_create_error("Error - UserName not sent");
        }

        picojson::object to_ret;
        try
        {
            std::string username = payload["UserName"].to_str();

            std::cout << "Requesting Almanac Server for User... " << username << std::endl;
            picojson::object user_resp = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host,
                                                                              mAlmanacServerCfg.port,
                                                                              "/persistence/users/"+username,
                                                                              PolyminisServer::HttpMethod::GET,
                                                                              picojson::object());
            auto response_v = picojson::value(user_resp);
            auto result = JsonHelpers::json_get_object(response_v, "Response");

            bool res = true;
            if (result.count("UserName") == 0)
            {
                res = false;
            }
            else
            {
                to_ret["UserName"] = picojson::value(result["UserName"]);
                sessionData.UserName = result["UserName"].to_str();
            }

            to_ret["Result"] = picojson::value(res);

            if (result.count("LastKnownPosition") > 0)
            {
                to_ret["LastKnownPosition"] =  result["LastKnownPosition"];
            }

            if (result.count("InventorySlots") > 0)
            {
                to_ret["InventorySlots"] =  result["InventorySlots"];
            }

        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
        }
        return std::move(to_ret);
    }
}

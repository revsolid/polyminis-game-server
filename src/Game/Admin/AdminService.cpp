#include "AdminService.h"

namespace Admin
{
    AdminService::AdminService(PolyminisServer::WSServer& server,
                               PolyminisServer::ServerCfg& almanacServer,
                               PolyminisGameRules::GameRules& gameRules) :
                               mGameRules(gameRules),
                               mAlmanacServerCfg(almanacServer)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "admin";
        wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sd)
        {
            return this->AdminEndpoint(request, sd);
        };
        server.AddService(wss);
    }
    picojson::object AdminService::AdminEndpoint(picojson::value& request, PolyminisServer::SessionData& sd)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_as_object(request);
        picojson::object toRet;

        if (command == "UPLOAD_GAME_RULES")
        {
            std::cout << "Updating Game Rules..." << std::endl;
            mGameRules.ReloadFromJson(request); 
        }
        if (command == "SAVE_CURRENT_TO_DB")
        {
            std::cout << "Saving current Game Rules to the Database..." << std::endl;
            mGameRules.SaveToDB(mAlmanacServerCfg);
        }

        if (command == "RELOAD_FROM_DB")
        {  
            std::cout << "Reloading Game Rules from the Database..." << std::endl;
            mGameRules.ReloadFromDB(mAlmanacServerCfg);
        }

        if (command == "GET_FROM_SERVER")
        {
            //
        }

        toRet["EventString"] = picojson::value("ADMIN_GAME_RULES");
        toRet["Rules"] = mGameRules.SerializeRules();
        return toRet;
    }
}

#include "SpeciesCatalogueService.h"
#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"

namespace SpeciesCatalogue
{
    SpeciesCatalogueService::SpeciesCatalogueService(PolyminisServer::WSServer& server,                                                     PolyminisServer::ServerCfg almanacServerCfg) :
                                                     PolyminisServer::ServerCfg almanacServerCfg) :
                                                     mAlmanacServerCfg(almanacServerCfg)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "species_catalogue";
        wss->mHandler = [=](picojson::value& request)
        {
            return this->SpeciesCatalogueEndpoint(request);
        };
        server.AddService(wss);
    }
    
    picojson::object SpeciesCatalogueService::SpeciesCatalogueEndpoint(picojson::value& request)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_object(request, "Payload");

        picojson::object to_ret;
        if (command == "SAVE_SPECIES")
        {
            std::cout << "SAVE_SPECIES command Received!" << std::endl;
            to_ret = picojson::object();
        }
        else
        {
            to_ret = JsonHelpers::json_create_error("Command Not Found");
        }
        return std::move(to_ret);
    }
    

}

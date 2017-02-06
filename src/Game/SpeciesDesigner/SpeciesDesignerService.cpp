#include "SpeciesDesignerService.h"

namespace SpeciesDesigner
{
    SpeciesDesignerService::SpeciesDesignerService(PolyminisServer::WSServer& server)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "species_designer";
        wss->mHandler = [=](picojson::value& request)
        {
            return this->SpeciesDesignerEndpoint(request);
        };
        server.AddService(wss);
    }
    
    picojson::object SpeciesDesignerService::SpeciesDesignerEndpoint(picojson::value& request)
    {
        std::string command = PolyminisServer::JsonHelpers::json_get_string(request, "Command");
        auto payload = PolyminisServer::JsonHelpers::json_get_object(request, "Payload");

        picojson::object to_ret;
        if (command == "SAVE_SPECIES")
        {
            std::cout << "SAVE_SPECIES command Received!" << std::endl;
            to_ret = picojson::object();
        }
        else
        {
            to_ret = PolyminisServer::JsonHelpers::json_create_error("Command Not Found");
        }
        return std::move(to_ret);
    }
    

}

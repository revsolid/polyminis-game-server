#include "SpeciesCatalogueService.h"
#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"

namespace SpeciesCatalogue
{
    SpeciesCatalogueService::SpeciesCatalogueService(PolyminisServer::WSServer& server,
                                                     PolyminisServer::ServerCfg almanacServerCfg) :
                                                     mAlmanacServerCfg(almanacServerCfg)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "species_catalogue";
        wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sd)
        {
            return this->SpeciesCatalogueEndpoint(request);
        };
        server.AddService(wss);
    }
    
    picojson::object SpeciesCatalogueService::SpeciesCatalogueEndpoint(picojson::value& request)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_as_object(request);

        if (payload.count("Species") == 0)
        {
            return JsonHelpers::json_create_error("Error - Species not sent");
        }
        auto species_value = picojson::value(payload);
        auto specieslist_json = JsonHelpers::json_get_array(species_value, "Species");
        picojson::object to_ret;
        if (command == "SAVE_SPECIES")
        {
            for (picojson::array::iterator iter = specieslist_json.begin(); iter != specieslist_json.end(); ++iter) 
            {
                std::cout << "Species Received: [" << (*iter).get("Name").get<std::string>() << "]"<< std::endl;
            }
            //std::cout << "SAVE_SPECIES command Received!" << std::endl;
            to_ret = picojson::object();
        }
        else
        {
            to_ret = JsonHelpers::json_create_error("Command Not Found");
        }
        return std::move(to_ret);
    }
    

}

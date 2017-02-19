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
            return this->SpeciesCatalogueEndpoint(request, sd);
        };
        server.AddService(wss);

        // Debug data, add splices to mSplices
        Splice s1;
        s1.mInternalName = "polar";
        Splice s2;
        s2.mInternalName = "tropical";
        Splice s3;
        s3.mInternalName = "hot_and_cold";
        Splice s4;
        s4.mInternalName = "g_eater";
        Splice s5;
        s5.mInternalName = "cryophile";
        Splice s6;
        s6.mInternalName = "thermophile";

        mCatalogueSession.AddSplice(s1);
        mCatalogueSession.AddSplice(s2);
        mCatalogueSession.AddSplice(s3);
        mCatalogueSession.AddSplice(s4);
        mCatalogueSession.AddSplice(s5);
        mCatalogueSession.AddSplice(s6);
    }
    
    picojson::object SpeciesCatalogueService::SpeciesCatalogueEndpoint(picojson::value& request,
                                                                    PolyminisServer::SessionData& sd)
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
            if(mCatalogueSession.AttemptSaveSpecies(specieslist_json))
            {
                std::cout << "New Species Saved to game server. " << std::endl;
                SaveInventoryOnDB(sd.UserName);
            }
            else
            {
                std::cout << "Failted saving to game server" << std::endl;
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
    
    bool SpeciesCatalogueService::GetSpeciesDateonDB()
    {
        try
        {
            std::cout << "Requesting Almanac Server for User Inventory species..." << std::endl;
            picojson::object planets_resp = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host,
                                                                                 mAlmanacServerCfg.port,
                                                                                 "/persistence/planets/",
                                                                                 PolyminisServer::HttpMethod::GET,
                                                                                 picojson::object());
                                                                        

            auto planets_resp_v = picojson::value(planets_resp);
            auto planets = JsonHelpers::json_get_object(planets_resp_v, "Response");
            auto planets_value = picojson::value(planets);
            picojson::array all_planets =  JsonHelpers::json_get_array(planets_value, "Items");

        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
        }
        return true;

    }

    bool SpeciesCatalogueService::SaveInventoryOnDB(const std::string& userName)
    {
        try
        {
            picojson::object invPayload;
            picojson::object invEntry;
            picojson::array species = mCatalogueSession.GetSpeciesJsonArray();

            invEntry["Type"] = picojson::value("SpeciesInStorage");
            invEntry["Value"] = picojson::value(species);

            invPayload["UserName"] = picojson::value(userName);
            invPayload["InventoryEntry"] = picojson::value(invEntry);
            PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, "/persistence/inventoryentries/"+userName,
                                                 PolyminisServer::HttpMethod::POST, invPayload);
        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;
            return false;
        }
        return true;
    }

}

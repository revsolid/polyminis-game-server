#include "InventoryService.h"
#include "Core/JsonHelpers.h"
#include "Core/HttpClient.h"
#include "Game/GameUtils.h"
#include <time.h>

namespace Inventory
{
    InventoryService::InventoryService(PolyminisServer::WSServer& server,
                                       PolyminisServer::ServerCfg& almanacServerCfg, 
                                       PolyminisGameRules::GameRules& gameRules) : mAlmanacServerCfg(almanacServerCfg), mGameRules(gameRules)
    {
        auto wss = std::make_shared<PolyminisServer::WSService>();
        wss->mServiceName = "inventory";
        wss->mHandler =  [=] (picojson::value& request, PolyminisServer::SessionData& sd)
        {
            return this->InventoryEndpoint(request, sd);
        };
        server.AddService(wss);
    }

    picojson::object InventoryService::InventoryEndpoint(picojson::value& request, PolyminisServer::SessionData& sd)
    {
        std::string command = JsonHelpers::json_get_string(request, "Command");
        auto payload = JsonHelpers::json_get_as_object(request);


        if (payload.count("Slot") == 0)
        {
            return JsonHelpers::json_create_error("Error - Slot not sent");
        }
        int slot  = JsonHelpers::json_get_int(request, "Slot");

        picojson::object toRet;
        bool return_inventory = command == "GET_INVENTORY";

        toRet["Service"] = picojson::value("inventory");
        if (command == "SAMPLE_FROM_PLANET" || command == "RESEARCH" || command == "NEW_SPECIES")
        {
            picojson::object invPayload;
            picojson::object invEntry;

            bool new_species = command == "NEW_SPECIES";
            // TODO: Shit load of anti-cheat stuff should happen here 
            // For now, we pray no one cheats xD
            //
            // Anti-cheat / validation (TODO)

            int epoch = 0;
            int pid = 0;

            if (payload.count("Species") == 0)
            {
                    return JsonHelpers::json_create_error("Error - PlanetId not sent");
            }

            picojson::value speciesData = picojson::value(JsonHelpers::json_get_object(request, "Species"));
            std::string speciesName = JsonHelpers::json_get_string(speciesData, "SpeciesName");

            if (!new_species)
            {
                if (payload.count("PlanetId") == 0)
                {
                    return JsonHelpers::json_create_error("Error - PlanetId not sent");
                }
                if (payload.count("Epoch") == 0)
                {
                    return JsonHelpers::json_create_error("Error - Epoch not sent");
                }

                epoch = JsonHelpers::json_get_int(request, "Epoch");
                pid   = JsonHelpers::json_get_int(request, "PlanetId");
            }

            if (command == "RESEARCH")
            {
                invEntry["InventoryType"] = picojson::value("Research");
                invEntry["Value"] = picojson::value(CreateResearchPayload(speciesData, pid, epoch, speciesName, sd));
            }  
            else
            {
                std::string planetEpoch = std::to_string(pid)+std::to_string(epoch);
                invEntry["InventoryType"] = picojson::value("SpeciesSeed");

                if (command == "SAMPLE_FROM_PLANET")
                {
                    speciesData = picojson::value(GameDBUtils::GetSpeciesInPlanet(mAlmanacServerCfg, planetEpoch, speciesName));
                }
                invEntry["Value"] = picojson::value(CreateSpeciesSeedPayload(speciesData, planetEpoch, speciesName, new_species, sd));
            }

            invPayload["UserName"] = picojson::value(sd.UserName);
            invPayload["Slot"] = picojson::value((double)slot);
            invPayload["InventoryEntry"] = picojson::value(invEntry);
            
            PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, "/persistence/inventoryentries/"+sd.UserName+"/"+std::to_string(slot),
                                                 PolyminisServer::HttpMethod::POST, invPayload);
            return_inventory = true;
        }
        else if (command == "UPDATE_SPECIES")
        {
            picojson::value speciesData = picojson::value(JsonHelpers::json_get_object(request, "Species"));
            std::string speciesName = JsonHelpers::json_get_string(speciesData, "SpeciesName");
            std::string planetEpoch = JsonHelpers::json_get_string(speciesData, "PlanetEpoch");

            picojson::object invPayload;
            invPayload["UserName"] = picojson::value(sd.UserName);
            invPayload["Slot"] = picojson::value((double)slot);

            picojson::object invEntry;
            invEntry["InventoryType"] = picojson::value("SpeciesSeed");
            invEntry["Value"] = picojson::value(CreateSpeciesSeedPayload(speciesData, planetEpoch, speciesName, false, sd));

            invPayload["InventoryEntry"] = picojson::value(invEntry);
            PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, "/persistence/inventoryentries/"+sd.UserName+"/"+std::to_string(slot),
                                                 PolyminisServer::HttpMethod::PUT, invPayload);
        
            return_inventory = true;
        }
        else if (command == "DELETE_ENTRY")
        {
            // Delete inventory Entry
            PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, "/persistence/inventoryentries/"+sd.UserName+"/"+std::to_string(slot),
                                                 PolyminisServer::HttpMethod::DELETE, picojson::object());
            return_inventory = true;
        }

        // ALWAYS Return the full inventory, it is a bit inneficient that we're reloading from the DB, but a Cache in Almanac would be the right answer //TODO-IN-A-MILLION-YEARS
        if (return_inventory)
        {

            picojson::object entries_resp = PolyminisServer::HttpClient::Request(mAlmanacServerCfg.host, mAlmanacServerCfg.port, "/persistence/inventoryentries/"+sd.UserName,
                                                                                 PolyminisServer::HttpMethod::GET, picojson::object());

            auto entries_resp_v = picojson::value(entries_resp);
            auto entries = JsonHelpers::json_get_object(entries_resp_v, "Response");
            auto entries_value = picojson::value(entries);

            //TODO: We should check for any Research that is done by now 
            
            toRet["EventString"] = picojson::value("InventoryUpdate");
            picojson::array flattenEntries;
            for (auto k : JsonHelpers::json_get_array(entries_value, "Items"))
            {
                auto ie = JsonHelpers::json_get_object(k, "InventoryEntry");
                ie["Slot"] = picojson::value(JsonHelpers::json_get_float(k, "Slot"));
                flattenEntries.push_back(picojson::value(ie));
            }
            toRet["InventoryEntries"] = picojson::value(flattenEntries);
        }
        else
        {
            return JsonHelpers::json_create_error("Error - Command Not Found");
        }

        return std::move(toRet);
    }

    picojson::object InventoryService::CreateResearchPayload(const picojson::value& speciesData, int pid, int epoch, const std::string& speciesName,
                                                             const PolyminisServer::SessionData& sd)
    {
// TODO: GAME_RULES - Tagging al places that have Game rules implemented for later tracking 
// Hardcoding it to something now, it should take into account a bunch of factors like player level
#define RESEARCH_EPOCHS 2
        picojson::object payload = CreateSpeciesSeedPayload(speciesData, std::to_string(pid) + std::to_string(epoch), speciesName, false, sd);

        payload["BeingResearched"] = picojson::value(true);
        payload["EpochStarted"] = picojson::value((double)epoch);
        payload["EpochDone"] = picojson::value((double)(epoch + RESEARCH_EPOCHS));
        return std::move(payload);
    }

    picojson::object InventoryService::CreateSpeciesSeedPayload(const picojson::value& speciesData, const std::string& pPlanetEpoch, const std::string& pSpeciesName, bool isNew,
                                                                const PolyminisServer::SessionData& sd)
    {
// TODO: GAME_RULES - Tagging al places that have Game rules implemented for later tracking 
//
// A Species Seed is the Translation Table + User Tunning (Instincts and GAParams) + A pointer to the Individuals of an original Species
// this is to avoid duplication and manipulation of Individuals (The biggest payload in the system)
// Once a creature is deployed (Copied into the SpeciesInPlanet Table, the Individuals Row is copied from the original Species)
// New creatures are seeded with 1 of few 'prebaked' creatures
        std::string speciesName = pSpeciesName;
        std::string planetEpoch = pPlanetEpoch;
        std::string originalSpeciesName = speciesName;

        picojson::object payload;

        if (isNew)
        {  
            // Hardcoded a specific species
            planetEpoch = "1414100"; 
            speciesName = JsonHelpers::json_get_string(speciesData, "SpeciesName");
            originalSpeciesName = "Test Species 1A";
            payload["CreatorName"] = picojson::value(sd.UserName);
        }

        payload["SpeciesName"] = picojson::value(speciesName);
        payload["PlanetEpoch"] = picojson::value(planetEpoch);
        payload["OriginalSpeciesName"] = picojson::value(originalSpeciesName);
        if (JsonHelpers::json_has_field(speciesData, "GAConfiguration"))
        {
            payload["GAConfiguration"] = picojson::value(JsonHelpers::json_get_object(speciesData, "GAConfiguration"));
        }
    //    payload["InstinctTuning"] = picojson::value(JsonHelpers::json_get_object(speciesData, "InstinctTuning"));
        payload["Splices"] = picojson::value(JsonHelpers::json_get_array(speciesData, "Splices"));

        return std::move(payload);
    }
}

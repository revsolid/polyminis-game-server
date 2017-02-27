#pragma once
#include "Core/Server.h"
#include "Game/GameRules.h"

namespace Inventory
{
    class InventoryService
    {
    public:
        InventoryService(PolyminisServer::WSServer& server,
                         PolyminisServer::ServerCfg& almanacServerCfg,
                         PolyminisGameRules::GameRules& gameRules);
        picojson::object InventoryEndpoint(picojson::value& command, PolyminisServer::SessionData& sd);
        bool GetSpeciesDateonDB();

    private:
        bool SaveInventoryEntryOnDB(const std::string& userName, int slot);

        picojson::object CreateResearchPayload(const picojson::value& speciesData, int pid, int epoch, const std::string& speciesName,
                                               const PolyminisServer::SessionData& sd);
        picojson::object CreateSpeciesSeedPayload(const picojson::value& speciesData, const std::string& planetEpoch, const std::string& speciesName, bool isNew,
                                                  const PolyminisServer::SessionData& sd);

        // Members
        PolyminisServer::ServerCfg& mAlmanacServerCfg;
        PolyminisGameRules::GameRules& mGameRules;
    };
}

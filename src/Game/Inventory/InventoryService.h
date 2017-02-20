#pragma once
#include "Core/Server.h"

namespace Inventory
{
    class InventoryService
    {
    public:
        InventoryService(PolyminisServer::WSServer& server,
                         PolyminisServer::ServerCfg almanacServerCfg);
        picojson::object InventoryEndpoint(picojson::value& command, PolyminisServer::SessionData& sd);
        bool GetSpeciesDateonDB();

    private:
        bool SaveInventoryEntryOnDB(const std::string& userName, int slot);

        picojson::object CreateResearchPayload(int pid, int epoch, const std::string& speciesName);
        picojson::object CreateSpeciesSeedPayload(const picojson::value& speciesData, int pid, int epoch, const std::string& speciesName, bool isNew,
                                                  const PolyminisServer::SessionData& sd);

        // Members
        PolyminisServer::ServerCfg mAlmanacServerCfg;
    };
}

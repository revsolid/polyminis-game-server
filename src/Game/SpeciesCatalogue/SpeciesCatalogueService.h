#pragma once
#include "Core/Server.h"
#include "SpeciesCatalogue.h"

namespace SpeciesCatalogue
{
    class SpeciesCatalogueService
    {
    public:
        SpeciesCatalogueService(PolyminisServer::WSServer& server,
                                PolyminisServer::ServerCfg almanacServerCfg);
        picojson::object SpeciesCatalogueEndpoint(picojson::value& command, PolyminisServer::SessionData& sd);
        bool GetSpeciesDateonDB();
        bool SaveInventoryOnDB(const std::string& userName);

    private:
        // Members
        PolyminisServer::ServerCfg mAlmanacServerCfg;
        SpeciesCatalogueSession mCatalogueSession;
    };
}


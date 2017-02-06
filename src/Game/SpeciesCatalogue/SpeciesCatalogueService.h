#pragma once
#include "Core/Server.h"

namespace SpeciesCatalogue
{
    class SpeciesCatalogueService
    {
    public:
        SpeciesCatalogueService(PolyminisServer::WSServer& server);
        picojson::object SpeciesCatalogueEndpoint(picojson::value& command);

    private:
        // Members
    };
}


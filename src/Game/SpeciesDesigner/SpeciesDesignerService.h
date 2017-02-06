#pragma once
#include "Core/Server.h"

namespace SpeciesDesigner
{
    class SpeciesDesignerService
    {
    public:
        SpeciesDesignerService(PolyminisServer::WSServer& server);
        picojson::object SpeciesDesignerEndpoint(picojson::value& command);

    private:
        // Members
    };
}


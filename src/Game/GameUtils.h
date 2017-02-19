#pragma once
#include "Core/Server.h"


// Utils like saving Biomass values in th DB

namespace GameUtils
{
    bool SaveBiomassValue(const PolyminisServer::SessionData& session, const PolyminisServer::ServerCfg& almanacServerCfg);
}


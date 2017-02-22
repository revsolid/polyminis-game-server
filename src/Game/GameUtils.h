#pragma once
#include "Core/Server.h"


// Utils like saving Biomass values in th DB
// This is quickly becoming 'GameDBUtils' should've abstracted that earlier #SadFace 

namespace GameUtils
{
    bool SaveBiomassValue(const PolyminisServer::SessionData& session, const PolyminisServer::ServerCfg& almanacServerCfg);
    picojson::array GetSpeciesInPlanet(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch);
    
}


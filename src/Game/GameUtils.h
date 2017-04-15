#pragma once
#include "Core/Server.h"


// Utils like saving Biomass values in th DB
// This is quickly becoming 'GameDBUtils' should've abstracted that earlier #SadFace 

namespace GameDBUtils
{
    bool SaveBiomassValue(const PolyminisServer::SessionData& session, const PolyminisServer::ServerCfg& almanacServerCfg);

    // These functions hit the speciessummaries endpoint in the DB which is a 'view' on top of speciesinplanet.
    // SpeciesSummaries will include everything in SpeciesInPlanet *EXCEPT* GeneticPayload and any other piece of data that is only required for 
    // simulating the Species - This division exists only to avoid moving big blobs of JSON back and forth unnecessarily
    picojson::array GetAllSpeciesInPlanet(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, bool summary = true);
    picojson::object GetSpeciesInPlanet(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, const std::string& speciesName);

    // These functions hit the SpeciesInPlanet db endpoint and will bring down ALL the data for the species, so use ONLY IF ALL THE DATA IS NEEDED 
    picojson::object GetSpeciesGeneticPayload(const PolyminisServer::ServerCfg& almanacServerCfg, const std::string& planetEpoch, const std::string& speciesName);

    // These functions hit the Epoch db endpoint use these for statistics that span several epochs.
    // It uses Planet as the Key and Epoch as the Range in Dynamo so querying several epochs on the same
    // planet is super cheap
    picojson::object GetEpochStatistics(const PolyminisServer::ServerCfg& almanacServerCfg, int planetId, int epoch);
    
    
    int GetGlobalEpoch(const PolyminisServer::ServerCfg& almanacServerCfg);
}


namespace GameSimUtils
{
    picojson::object CreateSimulationServer(const PolyminisServer::ServerCfg& simServerCfg);
    picojson::array GetSimulationSteps(const PolyminisServer::ServerCfg& simServerCfg, int simSessionId, int& epoch, int& inout_startAtStep, int maxSteps = 100);

    picojson::array GetSpecies(const PolyminisServer::ServerCfg& simServerCfg, int simSessionId, int epoch); 
    picojson::object GetEnvironment(const PolyminisServer::ServerCfg& simServerCfg, int simSessionId, int epoch); 

    picojson::object RunSimulation(const PolyminisServer::ServerCfg& simServerCfg, const PolyminisServer::ServerCfg& almanacServerCfg, const picojson::object& masterTT,
                                   int simSessionId, int planetId, int epoch);
}

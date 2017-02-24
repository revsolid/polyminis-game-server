#pragma once
#include "Core/Server.h"
#include "PlanetManager.h"
#include "SpaceMap.h"


namespace SpaceExploration
{
    class OrbitalInteractionsService 
    {
    public:
        OrbitalInteractionsService(PolyminisServer::WSServer& server,
                                   PlanetManager& pManager,
                                   PolyminisServer::ServerCfg almanacServerCfg);

        picojson::object OISEndpoint(picojson::value& request, PolyminisServer::SessionData& sd);

    private:
        PolyminisServer::ServerCfg mAlmanacServerCfg;
        PlanetManager& mPlanetManager;

        float BiomassToPopulationPercentage(float biomass) { return biomass; }
        float PopulationPercentageToBiomass(float percentage) { return percentage; }
    };
}

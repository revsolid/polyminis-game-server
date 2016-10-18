#pragma once
#include "Planet.h"
#include <vector>
#include <string>

class PlanetManager
{
public:
	PlanetManager(const std::initializer_list<std::pair<float, float>>& list);

	void AddPlanet(Planet inPlanet);
	void AddPlanet(float x, float y, unsigned int id);


	// Update visibility status of each planet, 
	// and push changed ones to spawn or despawn list.
	void UpdatePlanetsVisibility(float x, float y, float distance);
	
	// return a message string for Spawning/Despawning		
	// in the form of <despawn>[3,5,17]
	std::vector<std::string> SpawnStrings();
	std::string DespawnString();

private:
	std::vector<Planet> mPlanets;
	std::vector<Planet*> mToSpawn;
	std::vector<Planet*> mToDespawn;
};


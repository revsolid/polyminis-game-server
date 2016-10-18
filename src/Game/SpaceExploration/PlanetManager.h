#pragma once
#include "Planet.h"
#include "Core/Types.h"
#include <vector>
#include <string>

class PlanetManager
{
public:
	PlanetManager(const std::initializer_list<std::pair<float, float>>& list);

	void AddPlanet(Planet inPlanet);
	void AddPlanet(float x, float y, unsigned int id);
	
	// return a vector for each planet visible 
	// from a point
	std::vector<std::string> SpawnVisibleStrings(Coord inCoord, float distance);

private:
	std::vector<Planet> mPlanets;
};


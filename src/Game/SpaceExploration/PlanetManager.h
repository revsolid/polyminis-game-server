#pragma once
#include <picojson.h>
#include <string>
#include <vector>
#include "Planet.h"
#include "Core/Types.h"

class PlanetManager
{
public:
    PlanetManager(const std::initializer_list<std::pair<float, float>>& list);

    void AddPlanet(Planet inPlanet);
    void AddPlanet(float x, float y, unsigned int id);

	Planet* GetPlanet(unsigned int inId) const;
	Planet* GetPlanet(Coord point) const; // if there is a planet close to point

    // return a vector for each planet visible 
    // from a point
    picojson::array GetVisiblePlanets(Coord inCoord, float distance);

private:
    std::vector<Planet> mPlanets;
};


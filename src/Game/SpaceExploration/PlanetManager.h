#pragma once
#include <picojson.h>
#include <string>
#include <vector>
#include <memory>
#include "Planet.h"
#include "Core/Types.h"

class PlanetManager
{
public:
    static Planet USELESS;
    PlanetManager(const std::initializer_list<std::pair<float, float>>& list);

    void AddPlanet(Planet inPlanet);
    void AddPlanet(float x, float y, unsigned int id);

    Planet& GetPlanet(unsigned int inId);
    Planet& GetPlanet(Coord point); // if there is a planet close to point

    int GetNextPlanetId();

    // return a vector for each planet visible 
    // from a point
    picojson::array GetVisiblePlanets(Coord inCoord, float distance);

private:
    std::vector<Planet> mPlanets;
    int mNextPlanetId;
};


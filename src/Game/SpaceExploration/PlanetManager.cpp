#include "PlanetManager.h"

Planet PlanetManager::USELESS = Planet(0, 0, -1);

PlanetManager::PlanetManager(const std::initializer_list<std::pair<float, float>>& list) : mNextPlanetId(0)
{
    for (auto i : list)
    {
        unsigned int id = GetNextPlanetId();
        mPlanets.push_back(Planet(i.first, i.second, id));
    }
}    

Planet& PlanetManager::GetPlanet(unsigned int inId)
{
    for (auto& p : mPlanets)
    {
        if (p.GetID() == inId)
        {
            return p;
        }
    }
    return PlanetManager::USELESS;
}

Planet& PlanetManager::GetPlanet(Coord point)
{
    float threshold = 0.1f;
    for (auto& p : mPlanets)
    {
        if (Coord::Distance(p.GetPos(), point) < threshold)
        {
            return p;
		}
    }
    return PlanetManager::USELESS;
}


void PlanetManager::AddPlanet(Planet inPlanet)
{
    mNextPlanetId = inPlanet.GetID() + 1;
    mPlanets.push_back(inPlanet);
}

void PlanetManager::AddPlanet(float x, float y, unsigned int id)
{
    AddPlanet(Planet(x, y, id));
}

picojson::array PlanetManager::GetVisiblePlanets(Coord inCoord, float distance)
{
    picojson::array retVal;

    for (auto p : mPlanets)
    {
        if (p.IsVisible(inCoord, distance))
        {
            retVal.push_back(picojson::value(p.ToJson()));
        }
    }

    return retVal;
}

int PlanetManager::GetNextPlanetId()
{
    return mNextPlanetId++;
}

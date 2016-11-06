#include "PlanetManager.h"



PlanetManager::PlanetManager(const std::initializer_list<std::pair<float, float>>& list)
{
    unsigned int id = 0;
    for (auto i : list)
    {
        mPlanets.push_back(Planet(i.first, i.second, id));
        ++id;
    }
}

void PlanetManager::AddPlanet(Planet inPlanet)
{
    mPlanets.push_back(inPlanet);
}

void PlanetManager::AddPlanet(float x, float y, unsigned int id)
{
    mPlanets.push_back(Planet(x, y, id));
}

picojson::array PlanetManager::GetVisiblePlanets(Coord inCoord, float distance)
{
    picojson::array retVal;

    for (auto p : mPlanets)
    {
        if (p.IsVisible(inCoord, distance))
        {
            picojson::object obj;
            picojson::object position_obj;
            auto pos = p.GetPos();
            position_obj["x"] = picojson::value(pos.x);
            position_obj["y"] = picojson::value(pos.y);

            obj["ID"] = picojson::value((double)p.GetID());
            obj["SpaceCoords"] = picojson::value(position_obj);
            retVal.push_back(picojson::value(obj));
        }
    }

    return retVal;
}

#include "PlanetManager.h"



PlanetManager::PlanetManager(const std::initializer_list<std::pair<float, float>>& list) : mNextPlanetId(0)
{
    for (auto i : list)
    {
        unsigned int id = GetNextPlanetId();
        mPlanets.push_back(Planet(i.first, i.second, id));
    }
}    

std::shared_ptr<Planet> PlanetManager::GetPlanet(unsigned int inId) const
{
    for (auto p : mPlanets)
    {
        if (p.GetID() == inId)
        {
            return std::make_shared<Planet>(p);
        }
    }
    return nullptr;
}

std::shared_ptr<Planet> PlanetManager::GetPlanet(Coord point) const
{
    float threshold = 0.1f;
    for (auto p : mPlanets)
    {
        if (Coord::Distance(p.GetPos(), point) < threshold)
        {
            return std::make_shared<Planet>(p);
		}
    }
    return nullptr;
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
            picojson::object obj;
            picojson::object position_obj;
            auto pos = p.GetPos();
            position_obj["x"] = picojson::value(pos.x);
            position_obj["y"] = picojson::value(pos.y);

            obj["ID"] = picojson::value((double)p.GetID());
            obj["SpaceCoords"] = picojson::value(position_obj);
            obj["PlanetName"] = picojson::value(p.GetName());
            retVal.push_back(picojson::value(obj));
        }
    }

    return retVal;
}

int PlanetManager::GetNextPlanetId()
{
    return mNextPlanetId++;
}

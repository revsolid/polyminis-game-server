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

std::vector<std::string> PlanetManager::SpawnVisibleStrings(Coord inCoord, float distance)
{
	std::string msg;
	std::vector<std::string> retVal;
	for (auto p : mPlanets)
	{
		if (p.IsVisible(inCoord, distance))
		{
			msg = "<spawn>[";
			msg += std::to_string(p.GetPos().x);
			msg += ",";
			msg += std::to_string(p.GetPos().y);
			msg += ",";
			msg += std::to_string(p.GetID());
			msg += "]";
			retVal.push_back(msg);
		}
	}

	return retVal;
}




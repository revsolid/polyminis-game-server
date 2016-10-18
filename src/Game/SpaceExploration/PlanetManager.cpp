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

void PlanetManager::UpdatePlanetsVisibility(float x, float y, float distance)
{
	for ( auto& p : mPlanets)
	{
		bool isVisible = p.IsVisible();
		p.UpdateVisibility(x, y, distance);

		if (isVisible && !p.IsVisible())
		{
			mToDespawn.push_back(&p);
		}
		else if (!isVisible && p.IsVisible())
		{
			mToSpawn.push_back(&p);
		}
	}
}

std::vector<std::string> PlanetManager::SpawnStrings()
{
	std::string msg;
	std::vector<std::string> retVal;
	for (auto p : mToSpawn)
	{
		msg = "<spawn>[";
		msg += std::to_string(p->GetPos().x);
		msg += ",";
		msg += std::to_string(p->GetPos().y);
		msg += ",";
		msg += std::to_string(p->GetID());
		msg += "]";
		retVal.push_back(msg);
	}
	mToSpawn.clear();

	return retVal;
}

std::string PlanetManager::DespawnString()
{
	std::string retVal = "";
	if (!mToDespawn.empty())
	{
		retVal += "<despawn>[";
		for (auto p : mToDespawn)
		{
			retVal += std::to_string(p->GetID());
			retVal += ",";
		}
		retVal.pop_back();
		retVal += "]";

		mToDespawn.clear();
	}
	return retVal;
}



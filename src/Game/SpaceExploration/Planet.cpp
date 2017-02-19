#include "Planet.h"
#include "Core/JsonHelpers.h"



SpeciesSummary SpeciesSummary::FromJson(picojson::value& json)
{
    SpeciesSummary ss;
    ss.Percentage  = JsonHelpers::json_get_float(json, "Percentage");
    ss.Name = JsonHelpers::json_get_string(json, "Name");
    ss.CreatorName = JsonHelpers::json_get_string(json, "Creator");
    return ss;
}

Planet::Planet(float x, float y, int inID):
    mID(inID)
{
    mPosition.x = x;
    mPosition.y = y;
}

Planet::Planet(float x, float y, int inID, float t_min, float t_max, float ph_min, float ph_max, std::string name) :
    mID(inID), mPosition(x, y), mTemperature(t_min, t_max), mPh(ph_min, ph_max), mName(name) { }

Coord Planet::GetPos() const { return mPosition; }

int Planet::GetID() const { return mID; }

Range<float> Planet::GetTemperatureRange() const { return mTemperature; }
Range<float> Planet::GetPhRange() const { return mPh; }

const std::string& Planet::GetName() const { return mName; }

bool Planet::IsVisible(Coord viewPoint, float visibleDistance)
{
    if (Coord::Distance(viewPoint, mPosition) < visibleDistance)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<SpeciesSummary> Planet::GetSpeciesInPlanet()
{
    return mSpecies;
}

void Planet::AddSpecies(SpeciesSummary&& species)
{
    mSpecies.push_back(species);
}

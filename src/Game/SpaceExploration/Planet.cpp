#include "Planet.h"
#include "Core/JsonHelpers.h"



SpeciesSummary SpeciesSummary::FromJson(picojson::value& json)
{
    SpeciesSummary ss;
    ss.Percentage  = JsonHelpers::json_get_float(json, "Percentage");
    ss.Name = JsonHelpers::json_get_string(json, "SpeciesName");
    ss.CreatorName = JsonHelpers::json_get_string(json, "CreatorName");
    ss.PlanetEpoch = JsonHelpers::json_get_string(json, "PlanetEpoch");
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

Planet::Planet(const picojson::value& planetJson, int nextID)
{
    int pid = nextID;
    float x   = 0.0f;
    float y   = 0.0f;
    int   epoch = 0;

    float t_min = 0.0f;
    float t_max = 0.0f;
    float ph_min = 0.0f;
    float ph_max = 0.0f;

    std::string name = "Test Planet";
    if (JsonHelpers::json_has_field(planetJson, "SpacePosition"))
    {
        auto position_json = picojson::value(JsonHelpers::json_get_object(planetJson, "SpacePosition"));
        x = JsonHelpers::json_get_float(position_json, "x");
        y = JsonHelpers::json_get_float(position_json, "y");
    }

    if (JsonHelpers::json_has_field(planetJson, "PlanetId"))
    {
        pid = JsonHelpers::json_get_int(planetJson, "PlanetId");
    }

    
    if (JsonHelpers::json_has_field(planetJson, "Temperature"))
    {
        auto temp_json = picojson::value(JsonHelpers::json_get_object(planetJson, "Temperature"));
        t_min = JsonHelpers::json_get_float(temp_json, "Min");
        t_max = JsonHelpers::json_get_float(temp_json, "Max");
    }

    if (JsonHelpers::json_has_field(planetJson, "Ph"))
    {
        auto ph_json = picojson::value(JsonHelpers::json_get_object(planetJson, "Ph"));
        ph_min = JsonHelpers::json_get_float(ph_json, "Min");
        ph_max = JsonHelpers::json_get_float(ph_json, "Max");
    }

    if (JsonHelpers::json_has_field(planetJson, "Epoch"))
    {
        epoch = JsonHelpers::json_get_int(planetJson, "Epoch");
    }

    if (JsonHelpers::json_has_field(planetJson, "PlanetName"))
    {
        name = JsonHelpers::json_get_string(planetJson, "PlanetName");
    }
    mID = pid;
    mPosition = Coord(x, y);
    mTemperature = Range<float>(t_min, t_max);
    mPh = Range<float>(ph_min, ph_max);
    mName = std::string(name);
}

Coord Planet::GetPos() const { return mPosition; }

int Planet::GetID() const { return mID; }
int Planet::GetEpoch() const { return mEpoch; }

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

std::vector<SpeciesSummary>& Planet::GetSpeciesInPlanet()
{
    return mSpecies;
}

void Planet::AddSpecies(SpeciesSummary&& species)
{
    mSpecies.push_back(species);
}

void Planet::SwapSpecies(const picojson::array& species)
{
    mSpecies.clear();
    for (auto summary : species)
    {
        SpeciesSummary ss = SpeciesSummary::FromJson(summary);
        AddSpecies(std::move(ss));
    } 
}

picojson::object Planet::ToJson()
{
    picojson::object obj;

    obj["ID"] = picojson::value((double)GetID());
    obj["PlanetName"] = picojson::value(GetName());

    picojson::object position_obj;
    auto pos = GetPos();
    position_obj["x"] = picojson::value(pos.x);
    position_obj["y"] = picojson::value(pos.y);
    obj["SpaceCoords"] = picojson::value(position_obj);


    picojson::object temp_obj;
    auto temp = GetTemperatureRange();
    temp_obj["Min"] = picojson::value(temp.Min);
    temp_obj["Max"] = picojson::value(temp.Max);
    obj["Temperature"] = picojson::value(temp_obj);

    picojson::object ph_obj;
    auto ph = GetPhRange();
    ph_obj["Min"] = picojson::value(temp.Min); 
    ph_obj["Max"] = picojson::value(temp.Max);
    obj["Ph"] = picojson::value(ph_obj);

    picojson::array species_arr;
    for (auto s : GetSpeciesInPlanet())
    {
        picojson::object species_obj;
        species_obj["SpeciesName"] = picojson::value(s.Name);
        species_obj["CreatorName"] = picojson::value(s.CreatorName);
        species_obj["PlanetEpoch"] = picojson::value(s.PlanetEpoch);
        species_arr.push_back(picojson::value(species_obj));
    }
        
    obj["Species"] = picojson::value(species_arr);
    return obj;
}

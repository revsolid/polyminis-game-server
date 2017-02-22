#pragma once
#include "Core/Types.h"
#include "Core/JsonHelpers.h"
#include <string>
#include <vector>



struct SpeciesSummary
{
    float Percentage = 0.0f;
    std::string Name = "Test Species";
    std::string CreatorName = "Chronos";
    std::string PlanetEpoch = "0000";

    static SpeciesSummary FromJson(picojson::value& json);
};

//Individual planet data for planets in exploration
// TODO: this class should reflect PlanetModel from the client
class Planet
{
public:
    explicit Planet(float x, float y, int inID);
    explicit Planet(float x, float y, int inID, float t_min, float t_max, float ph_min, float ph_max, std::string mName);
    explicit Planet(const picojson::value& planetJson, int nextID);

    Coord GetPos() const;
    int GetID() const;
    int GetEpoch() const;
    const std::string& GetName() const;
    Range<float> GetTemperatureRange() const;
    Range<float> GetPhRange() const;

    std::vector<SpeciesSummary>& GetSpeciesInPlanet();
    void AddSpecies(SpeciesSummary&& species);
    void SwapSpecies(const picojson::array& species);


    // Pass in a position and check if it's visible. Right now it just calculates 
    // euclidean distance but later we can do optimization
    bool IsVisible(Coord viewPoint, float visibleDistance);

    picojson::object ToJson();
private:
    Coord mPosition;
    int mID;
    int mEpoch = 0;
    Range<float> mTemperature;
    Range<float> mPh;
    std::string       mName;
    std::vector<SpeciesSummary>  mSpecies;
};

#pragma once
#include "Core/Types.h"
#include <string>

//Individual planet data for planets in exploration
// TODO: this class should reflect PlanetModel from the client
class Planet
{
public:
    explicit Planet(float x, float y, int inID);
    explicit Planet(float x, float y, int inID, float t_min, float t_max, float ph_min, float ph_max, std::string mName);

    Coord GetPos() const;
    int GetID() const;
    const std::string& GetName() const;
    Range<float> GetTemperatureRange() const;
    Range<float> GetPhRange() const;

    // Pass in a position and check if it's visible. Right now it just calculates 
    // euclidean distance but later we can do optimization
    bool IsVisible(Coord viewPoint, float visibleDistance);
private:
    Coord mPosition;
    int mID;
    Range<float> mTemperature;
    Range<float> mPh;
    std::string       mName;
};

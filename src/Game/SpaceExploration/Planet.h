#pragma once
#include "Core/Types.h"

//Individual planet data for planets in exploration
// TODO: this class should reflect PlanetModel from the client
class Planet
{
public:
    explicit Planet(float x, float y, int inID);
    Coord GetPos() const;
    int GetID() const;

    // Pass in a position and check if it's visible. Right now it just calculates 
    // euclidean distance but later we can do optimization
    bool IsVisible(Coord viewPoint, float visibleDistance);
private:
    Coord mPosition;
    int mID;
};

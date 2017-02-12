#include "Planet.h"

Planet::Planet(float x, float y, int inID):
    mID(inID)
{
    mPosition.x = x;
    mPosition.y = y;
}

Planet::Planet(float x, float y, int inID, float t_min, float t_max, float ph_min, float ph_max, std::string name) :
    mID(inID), mPosition(x, y), mTemperature(t_min, t_max), mPh(ph_min, ph_max), mName(name) 
{
    
}

Coord Planet::GetPos() const
{
    return mPosition;
}

int Planet::GetID() const
{
    return mID;
}

const std::string& Planet::GetName() const
{
    return mName;
}

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

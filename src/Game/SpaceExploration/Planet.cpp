#include "Planet.h"

Planet::Planet(float x, float y, int inID):
    mID(inID)
{
    mPosition.x = x;
    mPosition.y = y;
}

Coord Planet::GetPos() const
{
    return mPosition;
}

int Planet::GetID() const
{
    return mID;
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

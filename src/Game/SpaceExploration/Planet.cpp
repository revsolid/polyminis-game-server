#include "Planet.h"

Planet::Planet(float x, float y, int inID):
	mID(inID)
{
	mPosition.x = x;
	mPosition.y = y;
}

Coord Planet::GetPos()
{
	return mPosition;
}

int Planet::GetID()
{
	return mID;
}

bool Planet::IsVisible(Coord viewPoint, float visibleDistance)
{
	if (sqrt(pow(mPosition.x - viewPoint.x, 2) + pow(mPosition.y - viewPoint.y, 2)) < visibleDistance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

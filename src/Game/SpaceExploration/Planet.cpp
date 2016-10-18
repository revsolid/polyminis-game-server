#include "Planet.h"

Planet::Planet(float x, float y, int inID):
	mID(inID),
	mIsVisible(false)
{
	mPosition.x = x;
	mPosition.y = y;
}

Vector2 Planet::GetPos()
{
	return mPosition;
}

int Planet::GetID()
{
	return mID;
}


void Planet::UpdateVisibility(float inX, float inY, float visibleDistance)
{
	if (sqrt(pow(mPosition.x - inX, 2) + pow(mPosition.y - inY, 2)) < visibleDistance)
	{
		mIsVisible = true;
	}
	else
	{
		mIsVisible = false;
	}
}

bool Planet::IsVisible()
{
	return mIsVisible;
}

#pragma once
#include <math.h>

struct Vector2
{
	float x = 0;
	float y = 0;
};

//Individual planet data for planets in exploration
class Planet
{
public:
	explicit Planet(float x, float y, int inID);
	Vector2 GetPos();
	int GetID();

	// Pass in a position and update isVisibility. Right now it just calculates 
	// euclidean distance but later we can do optimization
	void UpdateVisibility(float inX, float inY, float visibleDistance);
	bool IsVisible();
private:
	Vector2 mPosition;
	int mID;
	bool mIsVisible;
};
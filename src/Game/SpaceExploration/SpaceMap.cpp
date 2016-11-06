#include "SpaceMap.h"

Coord SpaceMapSession::GetPos()
{
	return mPosition;
}


SpaceMapSession::SpaceMapSession(Coord inCoord) :
	mPosition(inCoord)
{
	//TODO: assign different ID to players
	mPlayerId = 42;
}


bool SpaceMapSession::AttemptMove(Coord inCoord) 
{
	//TODO: right now it just always return true...
	mPosition = inCoord;

	return true;
}

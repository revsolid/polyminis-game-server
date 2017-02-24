#include "SpaceMap.h"

//TODO: This file seems to no longer make sense now with SessionData
// Should delete ?
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

bool SpaceMapSession::AttemptWarp(PlanetManager& pManager, Coord point)
{
    Planet dest = pManager.GetPlanet(point);

    // TODO: shouldn't be able to warp all the time
    if (dest.GetID() != -1)
    {
        mPosition = dest.GetPos();
        return true;
    }
    return false;
}

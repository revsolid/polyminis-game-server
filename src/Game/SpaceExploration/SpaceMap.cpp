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

bool SpaceMapSession::AttemptWarp(const PlanetManager& pManager, Coord point)
{
    Planet* dest = pManager.GetPlanet(point);

    // TODO: shouldn't be able to warp all the time
    if (dest != nullptr)
    {
        mPosition = dest->GetPos();
        return true;
    }
    return false;
}

#ifndef __SPACE_MAP
#define __SPACE_MAP

#include "Core/Types.h"
#include "PlanetManager.h"

#include <vector>

// Star Map Sector
struct SMSector
{
  int mId;
};

struct SpaceMap
{
  std::vector<SMSector> sectors;
};

// Spacemap keeps track of a player as it moves in space
class SpaceMapSession
{
public:
    explicit SpaceMapSession(Coord inCoord);
    SpaceMapSession() {}
    ~SpaceMapSession(){}

    bool AttemptMove(Coord inCoord);
    bool AttemptWarp(PlanetManager& pManager, Coord point);

    Coord GetPos();
    float GetVisibilityRange() { return 600.0f; } //TODO: Where does this come from?

private:
    int   mPlayerId { -1 };
    Coord mPosition { 0.0f, 0.0f };
};

#endif

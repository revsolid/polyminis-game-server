#ifndef __SPACE_MAP
#define __SPACE_MAP

#include "Core/Types.h"

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
    ~SpaceMapSession(){}

	bool AttemptMove(Coord inCoord);
	void AttemptTeleport() {};

	Coord GetPos();

private:
	int   mPlayerId;
	Coord mPosition;
};

#endif

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
  SpaceMapSession(){}
  ~SpaceMapSession(){}


  //TODO: This are def. not void
  void AttemptMove(Coord coord) {}
  void AttemptTeleport(){}

private:
  int   mPlayerId;
  Coord mLocation;
};

#endif

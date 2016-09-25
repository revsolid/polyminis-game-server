#ifndef __SPACE_MAP
#define __SPACE_MAP

#include "Core/Types.h"


// Star Map Sector
struct SMSector
{
  int mId;
};

// Spacemap keeps track of a player as it moves in space
class SpaceMap
{
public:
  SpaceMap(){}
  ~SpaceMap(){}


  //TODO: This are def. not void
  void AttemptMove(Coord coord) {}
  void AttemptTeleport(){}

private:
  int   mPlayerId;
  Coord mLocation;
};

#endif

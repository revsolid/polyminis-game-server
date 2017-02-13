FILE(GLOB SRC
    "CreatureObservation/*.h"
    "CreatureObservation/*.cpp"
    "SpaceExploration/*.h"
    "SpaceExploration/*.cpp"
    "User/*.h"
    "User/*.cpp"
)
ADD_LIBRARY(game ${SRC})

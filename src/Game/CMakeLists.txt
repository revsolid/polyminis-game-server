FILE(GLOB SRC
    "SpaceExploration/*.h"
    "SpaceExploration/*.cpp"
    "CreatureObservation/*.h"
    "CreatureObservation/*.cpp"
)
ADD_LIBRARY(game ${SRC})

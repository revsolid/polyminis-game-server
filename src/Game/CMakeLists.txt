FILE(GLOB SRC
    "CreatureObservation/*.h"
    "CreatureObservation/*.cpp"
    "SpeciesCatalogue/*.h"
    "SpeciesCatalogue/*.cpp"
    "SpaceExploration/*.h"
    "SpaceExploration/*.cpp"
    "User/*.h"
    "User/*.cpp"
    "*.cpp"
    "*.h"
)
ADD_LIBRARY(game ${SRC})

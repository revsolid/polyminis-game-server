FILE(GLOB SRC
    "SpaceExploration/*.h"
    "SpaceExploration/*.cpp"
    "CreatureObservation/*.h"
    "CreatureObservation/*.cpp"
    "SpeciesCatalogue/*.h"
    "SpeciesCatalogue/*.cpp"

)
ADD_LIBRARY(game ${SRC})

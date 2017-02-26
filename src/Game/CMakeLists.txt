FILE(GLOB SRC
    "Admin/*.h"
    "Admin/*.cpp"
    "CreatureObservation/*.h"
    "CreatureObservation/*.cpp"
    "Inventory/*.h"
    "Inventory/*.cpp"
    "SpaceExploration/*.h"
    "SpaceExploration/*.cpp"
    "User/*.h"
    "User/*.cpp"
    "*.cpp"
    "*.h"
)
ADD_LIBRARY(game ${SRC})

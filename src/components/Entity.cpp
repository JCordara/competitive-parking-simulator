#include "Entity.h"

// Initialize instance counter to 0
unsigned int Entity::instanceCounter = 0;


Entity::Entity() {
    entityID = Entity::instanceCounter++;
}

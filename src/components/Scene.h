#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

#include "Entity.h"


class Scene {
public:

    std::shared_ptr<Entity> createEntity();
    bool destroyEntityByID(unsigned int entityID);
    bool destroyEntity(std::shared_ptr<Entity> doomedEntity);
    std::shared_ptr<Entity> getEntityByID(unsigned int entityID);

    std::vector<std::shared_ptr<Entity>>& entities();

private:
    std::vector<std::shared_ptr<Entity>> _entities;
};


#endif // SCENE_H

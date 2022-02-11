#ifndef SCENE_H
#define SCENE_H

#include "Common.h"

/* Forward declaration of Entity class */
class Entity;


class Scene {
public:

    std::shared_ptr<Entity> createEntity();
    bool destroyEntity(std::shared_ptr<Entity> doomedEntity);

    std::vector<std::shared_ptr<Entity>>& entities();
    
    bool destroyEntityByID(unsigned int entityID);
    std::shared_ptr<Entity> getEntityByID(unsigned int entityID);


private:
    std::vector<std::shared_ptr<Entity>> _entities;
};


#endif // SCENE_H

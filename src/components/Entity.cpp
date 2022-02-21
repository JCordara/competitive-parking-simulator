#include "Entity.h"
#include "TransformComponent.h"


// Initialize instance counter to 0
unsigned int Entity::instanceCounter = 0;

// Iniitialize global null entity
NullEntity nullEntity;


Entity::Entity(Entity& parent) : _parent(parent) {
    _entityID = Entity::instanceCounter++;
    addComponent<TransformComponent>();
}

std::shared_ptr<Entity> Entity::addChild() {
    _children.emplace_back(make_shared<Entity>(*this));
    return _children.back();
}

bool Entity::removeChild(std::shared_ptr<Entity> doomedEntity) {
    for (auto it = _children.begin(); it != _children.end(); it++) {
        if ((*it).get() == doomedEntity.get()) {
            _children.erase(it);
            return true;
        }
    }
    Log::error("Entity::destroyChild()\nNo matching entity found. ID:  %d", 
                doomedEntity->id());
    return false;
}

std::vector<std::shared_ptr<Entity>>& Entity::children() {
    return _children;
}

bool Entity::removeChildByID(unsigned int entityID) {
    for (auto it = _children.begin(); it != _children.end(); it++) {
        if ((*it)->id() == entityID) {
            _children.erase(it);
            return true;
        }
    }
    Log::error("Entity::destroyChildByID(%d)\nNo entity exists with ID %d", 
                entityID, entityID);
    return false;
}

std::shared_ptr<Entity> Entity::getChildByID(unsigned int entityID) {
    for (auto it = _children.begin(); it != _children.end(); it++) {
        if ((*it)->id() == entityID) {
            return *it;
        }
    }
    Log::error("Entity::getChildByID(%d)\nNo entity exists with ID %d", 
                entityID, entityID);
    // Return an empty shared ptr
    std::shared_ptr<Entity> empty(nullptr);
    return empty;
}

// --- Top level Scene aliases ---
Scene::Scene() : Entity(nullEntity) {}

std::shared_ptr<Entity> Scene::addEntity() {
    return addChild();
}

bool Scene::removeEntity(std::shared_ptr<Entity> doomedChild) {
    return removeChild(doomedChild);
}

std::vector<std::shared_ptr<Entity>>& Scene::entities() {
    return children();
}

bool Scene::removeEntityByID(unsigned int entityID) {
    return removeChildByID(entityID);
}

std::shared_ptr<Entity> Scene::getEntityByID(unsigned int entityID) {
    return getChildByID(entityID);
}
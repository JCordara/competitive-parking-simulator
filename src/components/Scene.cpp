#include "Scene.h"

std::shared_ptr<Entity> Scene::createEntity() {
    _entities.emplace_back(std::make_shared<Entity>());
    return _entities.back();
}

bool Scene::destroyEntityByID(unsigned int entityID) {
    for (auto it = _entities.begin(); it != _entities.end(); it++) {
        if ((*it)->id() == entityID) {
            _entities.erase(it);
            return true;
        }
    }

    std::cerr << "\x1b[31m";    // Output in red
    std::cerr << "[Error] Scene::destroyEntityByID("
        << entityID << ")\nNo entity exists with ID " << entityID;
    std::cerr << "\x1b[0m\n";   // Reset output color

    return false;
}

bool Scene::destroyEntity(std::shared_ptr<Entity> doomedEntity) {
    for (auto it = _entities.begin(); it != _entities.end(); it++) {
        if ((*it).get() == doomedEntity.get()) {
            _entities.erase(it);
            return true;
        }
    }

    std::cerr << "\x1b[31m";    // Output in red
    std::cerr << "[Error] Scene::destroyEntity()\n"
        << "No matching entity found. ID: " << doomedEntity->id();
    std::cerr << "\x1b[0m\n";   // Reset output color

    return false;
}

std::shared_ptr<Entity> Scene::getEntityByID(unsigned int entityID) {
    for (auto it = _entities.begin(); it != _entities.end(); it++) {
        if ((*it)->id() == entityID) {
            return *it;
        }
    }

    std::cerr << "\x1b[31m";    // Output in red
    std::cerr << "[Error] Scene::getEntityByID("
        << entityID << ")\nNo entity exists with ID " << entityID;
    std::cerr << "\x1b[0m\n";   // Reset output color

    // Return an empty shared ptr
    std::shared_ptr<Entity> empty(nullptr);
    return empty;
}

std::vector<std::shared_ptr<Entity>>& Scene::entities() {
    return _entities;
}

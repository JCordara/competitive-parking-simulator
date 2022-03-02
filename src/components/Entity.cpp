#include "Entity.h"
#include "TransformComponent.h"


// Initialize instance counter to 0
unsigned int Entity::instanceCounter = 0;

// Iniitialize global null entity
NullEntity nullEntity;


Entity::Entity(sp<Entity> parent) 
    : _parent(parent), _siblingNumber(-1), _self(nullptr) {
    _entityID = Entity::instanceCounter++;
    addComponent<TransformComponent>();
}

sp<Entity> Entity::addChild() {
    _children.emplace_back(make_shared<Entity>(_self));
    _children.back()->_self = _children.back();
    _children.back()->_siblingNumber = _children.size() - 1;
    return _children.back();
}

bool Entity::removeChild(sp<Entity> doomedEntity) {
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

std::vector<sp<Entity>>& Entity::directChildren() {
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

sp<Entity> Entity::getChildByID(unsigned int entityID) {
    for (auto it = _children.begin(); it != _children.end(); it++) {
        if ((*it)->id() == entityID) {
            return *it;
        }
    }
    Log::error("Entity::getChildByID(%d)\nNo entity exists with ID %d", 
                entityID, entityID);
    // Return an empty shared ptr
    sp<Entity> empty(nullptr);
    return empty;
}


// --- Entity Iterator ---
Entity::Iterator Entity::begin() {
    // Return an iterator pointing to the first child of traversal
    if (!_children.empty())
        return Iterator(_children[0]);
    // Fail silently if no children to iterate through
    else
        return Iterator(nullptr);
}

Entity::Iterator Entity::end() {
    // Return an iterator pointing to the last entity of traversal
    // Which is the root of the tree to be traversed
    return Iterator(_self);
}

/** --- Traverse to next entity in hierarchy --- **
 * Case 0: Entity has children 
 *  -> return first child
 * Case 1: Entity has no children and has siblings following it
 *  -> return next sibling
 * Case 2: Entity has no children and no siblings following it
 *  -> return parent's next sibling
 * Case 3: Parent has no siblings following it
 *  -> travel up branch until parent is found that has sibling following
 *  -> if null entity found, return it
 */
Entity::Iterator& Entity::Iterator::operator++() {

    std::vector<sp<Entity>> siblings;

    if(_current->_parent != nullptr)
        siblings = _current->_parent->_children;

    /* Case 0 */
    if (_current->_children.empty() == false) {
        _current = _current->_children[0];
    }

    /* Case 1 */
    else if (siblings.size() > (_current->_siblingNumber + 1)) {
        _current = siblings[_current->_siblingNumber + 1];
    }

    else {
        /* Case 2 */
        // Point _current to parent entity
        _current = _current->_parent;

        // Check if we're done traversing a subtree
        if (_current.get() == _root.get()) {
            _current = _root;
            return *this;
        }

        /* case 3 */
        // Loop this until the nullptr parent is found (traversal finished)
        while (_current->_parent != nullptr) {

            // If parent entity has siblings following it
            siblings = _current->_parent->_children; // Update siblings vector
            if (siblings.size() > (_current->_siblingNumber + 1)) {
                _current = siblings[_current->_siblingNumber + 1];
                break;
            } else {
                _current = _current->_parent;
            }
        }
    }

    _visited.push_back(_current);
    return *this;
}

/* Traverse to previous entity in hierarchy */
Entity::Iterator& Entity::Iterator::operator--() {
    // If there are no previous visited entities, just fail silently :)
    if (_visited.empty()) return *this;

    _visited.pop_back();        // Remove last entity
    _current = _visited.back(); // Get new last entity
    return *this;
}

Entity::Iterator Entity::Iterator::operator++(int) {
    return operator++();
}

Entity::Iterator Entity::Iterator::operator--(int) {
    return operator--();
}

Entity& Entity::Iterator::operator*() {
    return *_current; // Reference to entity pointed to by iterator
}

sp<Entity> Entity::Iterator::operator->() {
    return _current;
}

bool Entity::Iterator::operator!=(Entity::Iterator other) {
    // Failsafe: If comparing to null entity, always return false
    if (other._current == nullptr) return false;
    // Compare underlying pointers
    return _current.get() != other._current.get();
}

// Create an iterator that points to the specified entity
Entity::Iterator::Iterator(sp<Entity> e) : _current(e), _root(nullptr) {
    // Automatically visit the first node
    _visited.push_back(e);
    // Set the root entity
    if (_current->_parent != nullptr)
        _root = _current->_parent;
}



// --- Top level Scene aliases ---

sp<Scene> Scene::newScene() {
    sp<Scene> s = make_shared<Scene>();
    s->_self = s;
    return s;
}

Scene::Scene() : Entity(nullptr) {}

sp<Entity> Scene::addEntity() {
    return addChild();
}

bool Scene::removeEntity(sp<Entity> doomedChild) {
    return removeChild(doomedChild);
}


std::vector<sp<Entity>>& Scene::topLevelEntities() {
    return directChildren();
}

bool Scene::removeEntityByID(unsigned int entityID) {
    return removeChildByID(entityID);
}

sp<Entity> Scene::getEntityByID(unsigned int entityID) {
    return getChildByID(entityID);
}
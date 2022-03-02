#include "Entity.h"
#include "TransformComponent.h"


// Initialize instance counter to 0
unsigned int Entity::instanceCounter = 0;

// Iniitialize global null entity
NullEntity nullEntity;


Entity::Entity(Entity& parent) : _parent(parent), _siblingNumber(0) {
    _entityID = Entity::instanceCounter++;
    addComponent<TransformComponent>();

    // Find which sibling this entity is
    auto siblings = _parent._children;
    for (unsigned int i = 0; i < siblings.size(); i++) {
        if (siblings[i]->id() == _entityID) _siblingNumber = i;
    }
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

std::vector<std::shared_ptr<Entity>>& Entity::directChildren() {
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


// --- Entity Iterator ---
Entity::Iterator Entity::begin() {
    // Return an iterator pointing to the first child of traversal
    if (!_children.empty())
        return Iterator(_children[0].get());
    // Fail silently if no children to iterate through
    else
        return Iterator(this);
}

Entity::Iterator Entity::end() {

    // Get last child
    shared_ptr<Entity> lastChild;

    if (!_children.empty())
        lastChild = _children.back();
    // Fail silently if no children to iterate through
    else
        return Iterator(this);

    // If last child has children, get last child again
    // "Bring me that child!" - Michael Jackson
    while (!lastChild->_children.empty()) {
        lastChild = lastChild->_children.back();
    }
    
    // Return an iterator pointing to the last child of traversal
    return Iterator(lastChild.get());
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

    auto siblings = _current->_parent._children;

    /* Case 0 */
    if (_current->_children.empty() == false) {
        _current = _current->_children[0].get();
    }

    /* Case 1 */
    else if (siblings.size() >= _current->_siblingNumber) {
        _current = siblings[_current->_siblingNumber + 1].get();
    }

    /* Case 2 */
    else {
        
        // Point _current to parent entity
        _current = &_current->_parent;

        /* case 3 */
        // Loop this until the null entity is found (traversal finished)
        while (_current->id() != NULL_ID) {

            // If parent entity has siblings following it
            siblings = _current->_parent._children; // Update siblings vector
            if (siblings.size() >= _current->_siblingNumber) {
                _current = siblings[_current->_siblingNumber + 1].get();
                break;
            } else {
                _current = &_current->_parent;
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

    _current = _visited.back(); // Get last entity
    _visited.pop_back();        // Remove last entity
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

Entity* Entity::Iterator::operator->() {
    return _current;
}

bool Entity::Iterator::operator!=(Entity::Iterator other) {
    // Failsafe: If comparing to null entity, always return false
    if (other._current->id() == NULL_ID) return false;
    // Compare underlying pointers
    return _current != other._current;
}

// Create an iterator that points to the specified entity
Entity::Iterator::Iterator(Entity* e) : _current(e) {
    _visited.push_back(e);
}



// --- Top level Scene aliases ---
Scene::Scene() : Entity(nullEntity) {}

std::shared_ptr<Entity> Scene::addEntity() {
    return addChild();
}

bool Scene::removeEntity(std::shared_ptr<Entity> doomedChild) {
    return removeChild(doomedChild);
}


std::vector<std::shared_ptr<Entity>>& Scene::topLevelEntities() {
    return directChildren();
}

bool Scene::removeEntityByID(unsigned int entityID) {
    return removeChildByID(entityID);
}

std::shared_ptr<Entity> Scene::getEntityByID(unsigned int entityID) {
    return getChildByID(entityID);
}
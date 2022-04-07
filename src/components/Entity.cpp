#include "Entity.h"
#include "TransformComponent.h"


// Initialize instance counter to 0
unsigned int Entity::instanceCounter = 0;


Entity::Entity(sp<Entity> parent) 
    : _parent(parent), _siblingNumber(-1) {
    _entityID = Entity::instanceCounter++;
}


weak_ptr<Entity> Entity::addChild() {
    auto self = shared_from_this();
    _children.emplace_back(make_shared<Entity>(self));
    _children.back()->_siblingNumber = _children.size() - 1;
    _children.back()->addComponent<TransformComponent>();
    return _children.back();
}

bool Entity::removeChild(weak_ptr<Entity> doomedEntity) {
    
    // Copy weak pointer to shared pointer
    auto dm = doomedEntity.lock();
    if (!dm) return false; // Entity is already deleted

    for (auto it = _children.begin(); it != _children.end(); it++) {
        if ((*it).get() == dm.get()) {
            // dm->killurself();
            _children.erase(it);
            return true;
        }
    }
    std::cerr << "Entity::destroyChild()\nNo matching entity found. ID: " << dm->id();
    return false;
}

std::vector<sp<Entity>>& Entity::directChildren() {
    return _children;
}


bool Entity::removeChildByID(unsigned int entityID) {
    for (auto it = _children.begin(); it != _children.end(); it++) {
        if ((*it)->id() == entityID) {
            // (*it)->killurself();
            _children.erase(it);
            return true;
        }
    }
    Log::error("Entity::destroyChildByID(%d)\nNo entity exists with ID %d", 
                entityID, entityID);
    return false;
}

weak_ptr<Entity> Entity::getChildByID(unsigned int entityID) {
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

sp<Scene> Entity::getScene() {
    // p = direct parent
    sp<Entity> p = _parent.lock();

    // Get top level parent
    while(p->parent().lock() != nullptr) {
        p = p->parent().lock();
    }

    // Return weak pointer to top level parent (Scene)
    return dynamic_pointer_cast<Scene>(p);
}


Entity::~Entity() {

    // Call child destructors
    _children.clear();

    // Delete components
    for (auto keyValue : _components) {
    
        // Get component type enum
        ComponentEnum type = keyValue.first;

        // Untrack component from scene
        switch(type) {
            case ComponentEnum::ai:
                untrackComponentFromScene<AiComponent>(keyValue.second);
                // removeComponent<AiComponent>();
                break;
            case ComponentEnum::audio:
                untrackComponentFromScene<AudioComponent>(keyValue.second);
                // removeComponent<AudioComponent>();
                break;
            case ComponentEnum::camera:
                untrackComponentFromScene<CameraComponent>(keyValue.second);
                // removeComponent<CameraComponent>();
                break;
            case ComponentEnum::controller:
                untrackComponentFromScene<ControllerComponent>(keyValue.second);
                // removeComponent<ControllerComponent>();
                break;
            case ComponentEnum::description:
                untrackComponentFromScene<DescriptionComponent>(keyValue.second);
                // removeComponent<DescriptionComponent>();
                break;
            case ComponentEnum::lighting:
                untrackComponentFromScene<LightingComponent>(keyValue.second);
                // removeComponent<LightingComponent>();
                break;
            case ComponentEnum::model:
                untrackComponentFromScene<ModelComponent>(keyValue.second);
                // removeComponent<ModelComponent>();
                break;
            case ComponentEnum::renderer:
                untrackComponentFromScene<RendererComponent>(keyValue.second);
                // removeComponent<RendererComponent>();
                break;
            case ComponentEnum::rigidbody:
                untrackComponentFromScene<RigidbodyComponent>(keyValue.second);
                // removeComponent<RigidbodyComponent>();
                break;
            case ComponentEnum::transform:
                untrackComponentFromScene<TransformComponent>(keyValue.second);
                // removeComponent<TransformComponent>();
                break;
            case ComponentEnum::vehicle:
                untrackComponentFromScene<VehicleComponent>(keyValue.second);
                // removeComponent<VehicleComponent>();
                break;
            case ComponentEnum::volumeTrigger:
                untrackComponentFromScene<VolumeTriggerComponent>(keyValue.second);
                // removeComponent<VolumeTriggerComponent>();
                break;
        }
    }

    // Delete components for real
    _components.clear();
}

#if ENTITY_ITERATOR
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
    return Iterator(shared_from_this());
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

    std::vector<weak_ptr<Entity>> siblings;

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

sp<Entity>& Entity::Iterator::operator*() {
    return _current.lock(); // Reference to entity pointed to by iterator
}

weak_ptr<Entity> Entity::Iterator::operator->() {
    return _current;
}

bool Entity::Iterator::operator!=(Entity::Iterator other) {
    // Failsafe: If comparing to null entity, always return false
    if (other._current.lock() == nullptr) return false;
    // Compare underlying pointers
    return _current.lock().get() != other._current.lock().get();
}

// Create an iterator that points to the specified entity
Entity::Iterator::Iterator(sp<Entity> e) : _current(e), _root(nullptr) {
    // Automatically visit the first node
    _visited.push_back(e);
    // Set the root entity
    if (_current->_parent != nullptr)
        _root = _current->_parent;
}
#endif


// --- Top level Scene aliases ---

Scene::Scene() : Entity(nullptr) {}

sp<Scene> Scene::getScene() { 
    return dynamic_pointer_cast<Scene>(shared_from_this()); 
}


weak_ptr<Entity> Scene::addEntity() {
    return addChild();
}

bool Scene::removeEntity(weak_ptr<Entity> doomedChild) {
    return removeChild(doomedChild);
}


std::vector<sp<Entity>>& Scene::topLevelEntities() {
    return directChildren();
}

bool Scene::removeEntityByID(unsigned int entityID) {
    return removeChildByID(entityID);
}

weak_ptr<Entity> Scene::getEntityByID(unsigned int entityID) {
    return getChildByID(entityID);
}

// Menu

Menu::Menu(int columnNum, int rowNum, float inButtonOffset) {
	columnMaxNum = columnNum;
	rowMaxNum = rowNum;
	buttonOffset = inButtonOffset;
	calculateElementPositions();
}

// Type list asserted to be in order
// If one column, top down order
// If multiple columns top to bottom, then left to right order, column first
void Menu::calculateElementPositions() {
	// The gui positions are 0-1
	float columnDelimiter = 1.f/(float)columnMaxNum;
	float rowDelimiter = 1.f/(float)rowMaxNum;
	// Left to right first
	for (int i = 0; i < columnMaxNum; i++) {
		std::vector<Element> innerList;
		for (int j = 0; j < rowMaxNum; j++) {
			Element el;
			// Delimiter is where a column/row begins/ends, middle of it is position
			el.positionX = ((i * columnDelimiter) + columnDelimiter/2 - buttonOffset);
			el.positionY = ((j * rowDelimiter) +  rowDelimiter/2);
			innerList.push_back(el);
		}
		layout.push_back(innerList);
	}
}

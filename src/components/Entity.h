#ifndef ENTITY_H
#define ENTITY_H

#include "Common.h"

#define NULL_ID UINT_MAX - 1

class BaseComponent;
enum class ComponentEnum;


// To make some lines of code shorter
using std::unordered_map, std::pair;         // Mapping
using std::shared_ptr,    std::make_shared;  // Shared pointers
using std::unique_ptr,    std::make_unique;  // Unique pointers
using std::enable_if_t,   std::is_base_of_v; // Template param type assertion
using std::dynamic_pointer_cast;             // Downcast smart pointers

/**
 * @brief Components are stored in a map structure with keys of type 
 * ComponentEnum. Entities can only have 1 of each component type.
 */
class Entity {
public:

    Entity(Entity& parent);
    Entity& parent() { return _parent; }

    // Return this entity's unique ID
    unsigned int id() { return _entityID; }
    
    template<class C, class... Args>    // Variable argument length
    enable_if_t<is_base_of_v<BaseComponent, C>, bool> // Return bool
    // ^ Assert that C is derived from BaseComponent
    addComponent(Args&&... args) {
        // Construct the component
        shared_ptr<BaseComponent> component = make_shared<C>(*this, args...);
        // Get the components type enum (used as map key)
        ComponentEnum ctype = C::getType();
        // Insert component into map
        auto results =  _components.insert( {ctype, component} );
        // insert() returns a pair with second item indicating success
        return results.second;
    }
    
    template<class C>   // Same deal as above but no variable arguments
    enable_if_t<is_base_of_v<BaseComponent, C>, bool> 
    removeComponent() {
        // Remove component from map
        size_t numErased = _components.erase(C::getType());
        return static_cast<bool>(numErased);
    }

    template<class C>
    enable_if_t<is_base_of_v<BaseComponent, C>, shared_ptr<C>>
    getComponent() {
        try {
            return dynamic_pointer_cast<C>(_components.at(C::getType()));
        } catch (std::out_of_range) {
            return nullptr;
        }
    }

    template<class C>
    enable_if_t<is_base_of_v<BaseComponent, C>, bool>
    hasComponent() {
        return static_cast<bool>(_components.count(C::getType()));
    }

    shared_ptr<Entity> addChild();
    bool removeChild(shared_ptr<Entity> doomedChild);

    std::vector<shared_ptr<Entity>>& directChildren();

    
    bool removeChildByID(unsigned int entityID);
    shared_ptr<Entity> getChildByID(unsigned int entityID);


    /* --- Entity iterator implementation --- */

    class Iterator {
    public:
        
        friend Entity;             // Give Entity access to private constructor

        Iterator& operator++();    // Traverse iterator to next entity
        Iterator  operator++(int); // Same (pre/postfix current behave the same)
        Iterator& operator--();    // Traverse iterator to previous entity
        Iterator  operator--(int); // Same (pre/postfix current behave the same)
        Entity&   operator*();     // Access the entity at current iteration
        Entity*   operator->();     // Overload class member access operator
        bool operator!=(Iterator); // Inequality comparison for iteration


    private:
        // Encapsulated constructor because why construct an iterator manually
        Iterator(Entity*);
        
        // The entity currently being pointed to
        // I could go on a long ramble about why I'm using raw pointers
        // but you'll just have to trust me on this one
        Entity* _current;

        // A list of entities visited to allow for quick reverse traversal
        std::vector<Entity*> _visited;

    };

    // Constructs an iterator at the beginning of the children tree
    Iterator begin();
    
    // Constructs an iterator at the end of the children tree
    Iterator end();



protected:
    // Structure containing an entities components
    unordered_map<ComponentEnum, shared_ptr<BaseComponent>> _components;
    
    // Reference to parent entity
    Entity& _parent;

    // List of child entities
    std::vector<shared_ptr<Entity>> _children;

    // Unique ID
    unsigned int _entityID;

    // Position in children vector of parent
    unsigned int _siblingNumber;

    // Instance counter used for unique IDs
    static unsigned int instanceCounter;

    
};

// Simplify the scene to just be another entity with function aliases
class Scene : public Entity {
public:

    Scene();

    shared_ptr<Entity> addEntity();
    bool removeEntity(shared_ptr<Entity> doomedChild);

    std::vector<shared_ptr<Entity>>& topLevelEntities();
    
    bool removeEntityByID(unsigned int entityID);
    shared_ptr<Entity> getEntityByID(unsigned int entityID);

};

// Null Entity class (used as parent of top level scene)
class NullEntity : public Entity {
public: 
    NullEntity() : Entity(*this) { _entityID = NULL_ID; }
};

// Global null entity
extern NullEntity nullEntity;

#endif // ENTITY_H

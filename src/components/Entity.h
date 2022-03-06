#ifndef ENTITY_H
#define ENTITY_H

#include "Common.h"

#define NULL_ID UINT_MAX - 1

template<class T> using sp = std::shared_ptr<T>; // Shorter alias for shared_ptr


class BaseComponent;
enum class ComponentEnum;



/**
 * @brief Components are stored in a map structure with keys of type 
 * ComponentEnum. Entities can only have 1 of each component type.
 */
class Entity {
public:

    Entity(sp<Entity> parent);
    sp<Entity> parent() { return _parent; }

    // Return this entity's unique ID
    unsigned int id() { return _entityID; }
    
    template<class C, class... Args>    // Variable argument length
    enable_if_t<is_base_of_v<BaseComponent, C>, sp<C>>
    // ^ Assert that C is derived from BaseComponent
    addComponent(Args&&... args) {
        // Construct the component
        sp<BaseComponent> component = make_shared<C>(*this, args...);
        // Get the components type enum (used as map key)
        ComponentEnum ctype = C::getType();
        // Insert component into map
        _components.insert( {ctype, component} );
        // Return shared pointer to new component
        return dynamic_pointer_cast<C>(component);
    }
    
    template<class C>   // Same deal as above but no variable arguments
    enable_if_t<is_base_of_v<BaseComponent, C>, bool> 
    removeComponent() {
        // Remove component from map
        size_t numErased = _components.erase(C::getType());
        return static_cast<bool>(numErased);
    }

    template<class C>
    enable_if_t<is_base_of_v<BaseComponent, C>, sp<C>>
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

    sp<Entity> addChild();
    bool removeChild(sp<Entity> doomedChild);

    std::vector<sp<Entity>>& directChildren();

    
    bool removeChildByID(unsigned int entityID);
    sp<Entity> getChildByID(unsigned int entityID);


    /* --- Entity iterator implementation --- */

    class Iterator {
    public:
        
        friend Entity;             // Give Entity access to private constructor

        Iterator& operator++();    // Traverse iterator to next entity
        Iterator  operator++(int); // Same (pre/postfix current behave the same)
        Iterator& operator--();    // Traverse iterator to previous entity
        Iterator  operator--(int); // Same (pre/postfix current behave the same)
        Entity&   operator*();     // Access the entity at current iteration
        sp<Entity> operator->();   // Overload class member access operator
        bool operator!=(Iterator); // Inequality comparison for iteration


    private:
        // Encapsulated constructor because why construct an iterator manually
        Iterator(sp<Entity>);
        
        // The entity currently being pointed to
        sp<Entity> _current;

        // A list of entities visited to allow for quick reverse traversal
        std::vector<sp<Entity>> _visited;

        // Pointer to the root entity of the (sub-)tree being traversed
        sp<Entity> _root;
    };

    // Constructs an iterator at the beginning of the children tree
	Entity::Iterator begin();
    
    // Constructs an iterator at the end of the children tree
	Entity::Iterator end();



protected:
    // Structure containing an entities components
    unordered_map<ComponentEnum, sp<BaseComponent>> _components;
    
    // Reference to self
    sp<Entity> _self;

    // Reference to parent entity
    sp<Entity> _parent;

    // List of child entities
    std::vector<sp<Entity>> _children;

    // Unique ID
    unsigned int _entityID;

    // Position in children vector of parent
    int _siblingNumber;

    // Instance counter used for unique IDs
    static unsigned int instanceCounter;

    
};

// Simplify the scene to just be another entity with function aliases
class Scene : public Entity {
public:

    static sp<Scene> newScene();

    Scene();

    sp<Entity> addEntity();
    bool removeEntity(sp<Entity> doomedChild);

    std::vector<sp<Entity>>& topLevelEntities();
    
    bool removeEntityByID(unsigned int entityID);
    sp<Entity> getEntityByID(unsigned int entityID);

};

// Null Entity class (used as parent of top level scene)
class NullEntity : public Entity {
public: 
    NullEntity() : Entity(*this) { _entityID = NULL_ID; }
};

// Global null entity
extern NullEntity nullEntity;

#endif // ENTITY_H

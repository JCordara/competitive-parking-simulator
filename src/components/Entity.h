#ifndef ENTITY_H
#define ENTITY_H

#include "Common.h"

#define NULL_ID UINT_MAX - 1

template<class T> using sp = std::shared_ptr<T>; // Shorter alias for shared_ptr


class Scene;
class BaseComponent;
enum class ComponentEnum;



/**
 * @brief Components are stored in a map structure with keys of type 
 * ComponentEnum. Entities can only have 1 of each component type.
 */
class Entity : public std::enable_shared_from_this<Entity> {
public:

    Entity(sp<Entity> parent);
    sp<Entity> parent() { return _parent; }

    // Return this entity's unique ID
    unsigned int id() { return _entityID; }
    
    template<class C, class... Args>    // Variable argument length
    enable_if_t<is_base_of_v<BaseComponent, C>, sp<C>>
    // ^ Assert that C is derived from BaseComponent
    addComponent(Args&&... args) {
        // If component already exists, return existing component
        if (auto _c = getComponent<C>()) return _c;
        // Construct the component
        auto self = shared_from_this();
        sp<C> component = make_shared<C>(self, args...);
        // Insert component into map using component type enum as key
        _components.insert( {C::getType(), component} );
        // Track newly added component in scene
        getScene()->trackComponent<C>(component);
        // Return shared pointer to new component
        return component;
    }
    
    template<class C>   // Same deal as above but no variable arguments
    enable_if_t<is_base_of_v<BaseComponent, C>, bool> 
    removeComponent() {
        // Attempt to retrieve component to remove
        sp<C> component = getComponent<C>();
        // Return false if no matching component found
        if (!component) return false;
        // Remove component from map and retrieve success
        bool erased = static_cast<bool>(_components.erase(C::getType()));
        // Untrack component in scene and retrieve success
        bool untracked = getScene()->untrackComponent<C>(component);
        // Return true only if the component was removed from the entity
        // and untracked from the scene
        return (erased && untracked);
    }

    template<class C>
    enable_if_t<is_base_of_v<BaseComponent, C>, sp<C>>
    getComponent() {
        // Check if the entity has the requested component
        if (!static_cast<bool>(_components.count(C::getType())))
            return nullptr;
        // Return the requested component
        return dynamic_pointer_cast<C>(_components.at(C::getType()));
    }

    template<class C>
    enable_if_t<is_base_of_v<BaseComponent, C>, bool>
    hasComponent() {
        return static_cast<bool>(_components.count(C::getType()));
    }

    sp<Entity> addChild();
    bool removeChild(sp<Entity> doomedChild);

    std::vector<sp<Entity>>& directChildren();

    // Returns a pointer to the root node of hierarchy (Scene)
    virtual sp<Scene> getScene();
    
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
        shared_ptr<Entity>&   operator*();     // Access the entity at current iteration
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

    Scene();

    template<class C>
    vector<sp<C>>& iterate() {
        // If no components of this type are currently being tracked
        if (!static_cast<bool>(componentMap.count(C::getType()))) {
            // Create an empty list of components
            componentMap[C::getType()] = vector<sp<C>>();
        }
        // Return the vector of components
        return std::any_cast<vector<sp<C>>&>(componentMap.at(C::getType()));
    }

    template<class C>
    void trackComponent(sp<C> c) {
        // If no components of this type are currently being tracked
        if (!static_cast<bool>(componentMap.count(C::getType()))) {
            // Create an empty list of components
            componentMap[C::getType()] = vector<sp<C>>();
        }
        // Get the vector of components
        vector<sp<C>>& components = std::any_cast<vector<sp<C>>&>(componentMap.at(C::getType()));
        // Add the passed component to the vector
        components.push_back(c);
    }

    template<class C>
    bool untrackComponent(sp<C> c) {
        // If no components of this type are currently being tracked
        if (!static_cast<bool>(componentMap.count(C::getType()))) {
            return false; // Do nothing
        }
        // Get the vector of components
        vector<sp<C>>& components = std::any_cast<vector<sp<C>>&>(componentMap.at(C::getType()));
        // Search the vector for a matching component (by underlying shared_ptr address)
        for (auto it = components.begin(); it != components.end(); it++) {
            if (it->get() == c.get()) {
                components.erase(it);   // Erase matching component
                return true;
            }
        }
        return false;
    }

    sp<Scene> getScene();

    sp<Entity> addEntity();
    bool removeEntity(sp<Entity> doomedChild);

    std::vector<sp<Entity>>& topLevelEntities();
    
    bool removeEntityByID(unsigned int entityID);
    sp<Entity> getEntityByID(unsigned int entityID);

private:

    unordered_map<ComponentEnum, std::any> componentMap;

};

class Menu : public Entity {
public: 
	enum ElementType {
		BUTTON = 1,
		DROPDOWN,
		SLIDER
	};

	// Column -> row order
	int columnMaxNum;
	int rowMaxNum;
	// Pressing a button will move over a column/row
	std::vector<std::vector<ElementType>> layout;
	Menu();

};


#endif // ENTITY_H

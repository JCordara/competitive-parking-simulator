#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "Components.h"

// To make some lines of code shorter
using std::unordered_map, std::pair;         // Mapping
using std::shared_ptr,    std::make_shared;  // Shared pointers
using std::unique_ptr,    std::make_unique;  // Unique pointers
using std::enable_if_t,   std::is_base_of_v; // Template parame type assertion
using std::dynamic_pointer_cast;             // Downcast smart pointers

/**
 * @brief Components are stored in a map structure with keys of type 
 * ComponentEnum. Entities can only have 1 of each component type.
 */
class Entity {
public:
    Entity();
    
    template<class C, class... Args>    // Variable argument length
    enable_if_t<is_base_of_v<BaseComponent, C>, bool> // Return bool
    // ^ Assert that C is derived from BaseComponent
    addComponent(Args&&... args) {
        // Construct the component
        shared_ptr<BaseComponent> component = make_shared<C>(args...);
        // Get the components type enum (used as map key)
        ComponentEnum ctype = C::getType();
        // Insert component into map
        auto results =  components.insert( {ctype, component} );
        // insert() returns a pair with second item indicating success
        return results.second;
    }
    
    template<class C>   // Same deal as above but no variable arguments
    enable_if_t<is_base_of_v<BaseComponent, C>, bool> removeComponent() {
        // Remove component from map
        size_t numErased = components.erase(C::getType());
        return static_cast<bool>(numErased);
    }

    template<class C>
    enable_if_t<is_base_of_v<BaseComponent, C>, shared_ptr<C>>
    getComponent() {
        try {
            return dynamic_pointer_cast<C>(components.at(C::getType()));
        } catch (std::out_of_range) {
            std::cerr << "\x1b[31m";    // Output in red
            std::cerr << "[Error] Entity does not have a \"" 
                << typeid(C).name()
                << "\" component";
            std::cerr << "\x1b[0m\n";   // Reset output color
            return nullptr;
        }
    }


private:
    // Structure containing an entities components
    unordered_map<ComponentEnum, shared_ptr<BaseComponent>> components;
};

#endif // ENTITY_H

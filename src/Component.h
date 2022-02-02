#ifndef COMPONENT_H
#define COMPONENT_H


typedef enum class Component {
    ai,
    audioSource,
    controller,
    model,          // Split this up into mesh + texture?
    physics,        // Is Rigidbody a better name?
    transform
} ComponentEnum;    // Alias for code clarity


/**
 * @brief Components must be derived from this interface. Components must
 * implement a static getType() class method that returns the appropriate 
 * ComponentEnum value.
 */
class BaseComponent {
public:
    virtual ~BaseComponent() = 0;
};

/**
 * @brief Example component that implements the BaseComponent interface.
 */
class TransformComponent : public BaseComponent {
public:
    
    TransformComponent();

    static  ComponentEnum getType();
    virtual ~TransformComponent();


    // Test attributes
    float x, y, z;

    TransformComponent(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }
};

#endif // COMPONENT_H

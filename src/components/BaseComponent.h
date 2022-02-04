#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

typedef enum class Component {
    ai,
    audio,
    controller,     // Haven't created a controller class yet, needed?
    model,
    physics,        // Is Rigidbody a better name?
    renderer,
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

#endif // BASE_COMPONENT_H

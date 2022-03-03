#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Components.h"


class TransformComponent : public BaseComponent {
public:
    
    TransformComponent(Entity& entity);
    static  ComponentEnum getType();
    ~TransformComponent();

    glm::vec3 getRelativePosition() { return _position; }
    glm::quat getRelativeRotation() { return _rotation; }
    glm::vec3 getRelativeScale()    { return _scale; }
    
    glm::vec3 getPosition();
    glm::quat getRotation();
    glm::vec3 getScale();
    
    void setPosition(glm::vec3 position);
    void setPosition(float x, float y, float z);
    // void setRotation(glm::quat rotation);
    void setScale(glm::vec3 scale);
    void setScale(float x, float y, float z);

    void translate(glm::vec3 translation);
    void translate(float x, float y, float z);
    void rotate(float rotation, glm::vec3 axis);
    void scale(glm::vec3 scale);
    void scale(float x, float y, float z);

    glm::mat4 getMatrix() { return parentMatrix(-1) * _matrix; }
    glm::mat4 parentMatrix(int depth);

private:
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
    glm::mat4 _matrix;
};

#endif // TRANSFORM_COMPONENT_H

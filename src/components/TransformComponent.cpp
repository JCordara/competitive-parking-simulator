#include "TransformComponent.h"

TransformComponent::TransformComponent(Entity& e) 
    : BaseComponent(e)
    , _position(glm::vec3())
    , _rotation(glm::quat())
    , _scale(glm::vec3())
    , _matrix(glm::mat4(1.0f))
{}

// Returns the identity matrix if parent is null or has no transform component
glm::mat4 TransformComponent::parentMatrix(int depth) {

    // Base case
    if (depth == 0) return _matrix;

    // Check that entity has parent
    shared_ptr<Entity> parent = entity.parent();
    if (parent == nullptr) return glm::mat4(1.0f);

    // Check that parent has transform component
    auto t = parent->getComponent<TransformComponent>();
    if(!t) return glm::mat4(1.0f);
    
    return t->parentMatrix(depth - 1) * _matrix;
}

glm::vec3 TransformComponent::getPosition() { 
    glm::mat4& p = parentMatrix(-1);
    glm::vec3 v = glm::vec3(p[3]);
    return v;
}

// glm::quat TransformComponent::getRotation() { 
//     return glm::quat(parentMatrix(-1));
// }

glm::vec3 TransformComponent::getScale()    { 
    glm::mat4& p = parentMatrix(-1);
    return glm::vec3(p[0][0], p[1][1], p[2][2]);
}


void TransformComponent::setPosition(glm::vec3 position) {
    _position = position;
    _matrix = glm::translate(glm::mat4(1.0f), position);
}

void TransformComponent::setPosition(float x, float y, float z) {
    setPosition(glm::vec3(x, y, z));
}

// void TransformComponent::setRotation(glm::quat rotation) {
//      _rotation = rotation;
//      _matrix = glm::rotate(?);
// }

void TransformComponent::setScale(glm::vec3 scale) {
    _scale = scale;
    _matrix = glm::scale(glm::mat4(1.0f), scale);
}

void TransformComponent::setScale(float x, float y, float z) {
    setScale(glm::vec3(x, y, z));
}

void TransformComponent::translate(glm::vec3 translation) {
    _position += translation;

    _matrix = glm::translate(_matrix, translation);
}

void TransformComponent::translate(float x, float y, float z) {
    translate(glm::vec3(x, y, z));
}

void TransformComponent::rotate(float rotation, glm::vec3 axis) {
    // _rotation += glm::quat(?);
    _matrix = glm::rotate(_matrix, rotation, axis);
}

void TransformComponent::scale(glm::vec3 scale) {
    _scale += scale;
    _matrix = glm::scale(_matrix, scale);
}

void TransformComponent::scale(float x, float y, float z) {
    scale(glm::vec3(x, y, z));
}



ComponentEnum TransformComponent::getType() {
    return ComponentEnum::transform;
}

TransformComponent::~TransformComponent() {
    // Nothing to do here yet
}

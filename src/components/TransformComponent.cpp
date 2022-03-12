#include "TransformComponent.h"

TransformComponent::TransformComponent(shared_ptr<Entity> e) 
    : BaseComponent(e)
    , _position(glm::vec3(0.f))
    , _rotation(physx::PxQuat(0.f, physx::PxVec3(1.f, 0.f, 0.f)))
    , _scale(glm::vec3(1.f))
{}

ComponentEnum TransformComponent::getType() {
    return ComponentEnum::transform;
}

TransformComponent::~TransformComponent() {
    // Nothing to do here yet
}


/* Member functions */
// ----------------

glm::vec3 TransformComponent::getGlobalPosition() {
	return glm::vec3(getGlobalMatrix() * glm::vec4(0.f, 0.f, 0.f, 1.f));
}

// Returns the identity matrix if parent is null or has no transform component
glm::mat4 TransformComponent::getNestedMatrix(int depth) {

    // Base case
    if (depth == 0) return getLocalMatrix();

    // Check that entity has parent
	shared_ptr<TransformComponent> t;
	shared_ptr<Entity> nextParent = entity;
	do {
		nextParent = nextParent->parent();
		if (nextParent == nullptr) return getLocalMatrix();
		t = nextParent->getComponent<TransformComponent>();
		// Check that parent has transform component, if not move to the next parent
	} while (!t);
    
    return t->getNestedMatrix(depth - 1) * getLocalMatrix();
}

glm::mat4 TransformComponent::onlyPositionTransformGlobal() {
	return onlyPositionTransformMatrix(-1);
}

// Returns the identity matrix if parent is null or has no transform component
glm::mat4 TransformComponent::onlyPositionTransformMatrix(int depth) {

	// Base case
	if (depth == 0) return getLocalTranslationMatrix();

	// Check that entity has parent
	shared_ptr<TransformComponent> t;
	shared_ptr<Entity> nextParent = entity;
	do {
		nextParent = nextParent->parent();
		if (nextParent == nullptr) return getLocalMatrix();
		t = nextParent->getComponent<TransformComponent>();
		// Check that parent has transform component, if not move to the next parent
	} while (!t);

	return t->onlyPositionTransformMatrix(depth - 1) * getLocalTranslationMatrix();
}

void TransformComponent::setLocalPosition(glm::vec3 position) {
    _position = position;
	updateComponents();
}

void TransformComponent::setLocalPosition(physx::PxVec3 position) {
    setLocalPosition(position.x, position.y, position.z);
}

void TransformComponent::setLocalPosition(float x, float y, float z) {
    setLocalPosition(glm::vec3(x, y, z));
}


void TransformComponent::setLocalRotation(physx::PxQuat rotation) {
      _rotation = rotation;
	  updateComponents();
}

void TransformComponent::setLocalRotation(float rotation, physx::PxVec3 axis) {
	setLocalRotation(physx::PxQuat(rotation, axis));
}

void TransformComponent::setLocalRotation(float rotation, glm::vec3 axis) {
	setLocalRotation(physx::PxQuat(rotation, physx::PxVec3(axis.x, axis.y, axis.z)));
}


void TransformComponent::setLocalScale(glm::vec3 scale) {
    _scale = scale;
}

void TransformComponent::setLocalScale(float x, float y, float z) {
    setLocalScale(glm::vec3(x, y, z));
}


void TransformComponent::localTranslate(glm::vec3 translation) {
    _position += translation;
	updateComponents();
}

void TransformComponent::localTranslate(float x, float y, float z) {
	localTranslate(glm::vec3(x, y, z));
}


void TransformComponent::localRotate(physx::PxQuat rotation) {
	_rotation = rotation *_rotation;
	updateComponents();
}

void TransformComponent::localRotate(float rotation, physx::PxVec3 axis) {
	localRotate(physx::PxQuat(rotation, axis));
}

void TransformComponent::localRotate(float rotation, glm::vec3 axis) {
	localRotate(physx::PxQuat(rotation, physx::PxVec3(axis.x, axis.y, axis.z)));
}


void TransformComponent::localScale(glm::vec3 scale) {
    _scale *= scale;
}

void TransformComponent::localScale(float x, float y, float z) {
	localScale(glm::vec3(x, y, z));
}


glm::mat4 TransformComponent::getLocalTranslationMatrix() {
	return glm::translate(glm::mat4(1.f), _position);
}

glm::mat4 TransformComponent::getLocalRotationMatrix() {
	return convert<glm::mat4>(_rotation);
}

glm::mat4 TransformComponent::getLocalScaleMatrix() {
	return glm::scale(glm::mat4(1.f), _scale);
}

glm::mat4 TransformComponent::getLocalMatrix() {
	return	  getLocalTranslationMatrix()
			* getLocalRotationMatrix()
			* getLocalScaleMatrix()
		;
}


void TransformComponent::updateComponents() {
	if (auto vc = entity->getComponent<VehicleComponent>()) {
		vc->setTransform(convert<physx::PxTransform>(getGlobalMatrix()));
	}
	if (auto ac = entity->getComponent<AudioComponent>()) {
		ac->updatePosition(getGlobalPosition());
	}
}


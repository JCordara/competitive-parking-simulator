#include "TransformComponent.h"

TransformComponent::TransformComponent(Entity& e) 
    : BaseComponent(e)
    , _position(glm::vec3(0.f))
    , _rotation(physx::PxQuat(0.f, physx::PxVec3(1.f, 0.f, 0.f)))
    , _scale(glm::vec3(1.f))
{}

glm::mat4 TransformComponent::getLocalTranslationMatrix() {
	return glm::translate(glm::mat4(1.f), _position);
}
glm::mat4 TransformComponent::getLocalRotationMatrix() {
	return quat_to_mat4(_rotation);
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

// Returns the identity matrix if parent is null or has no transform component
glm::mat4 TransformComponent::getNestedMatrix(int depth) {

    // Base case
    if (depth == 0) return getLocalMatrix();

    // Check that entity has parent
	std::shared_ptr<TransformComponent> t;
	do {
		shared_ptr<Entity> parent = entity.parent();
		if (parent == nullptr) return getLocalMatrix();
		t = parent->getComponent<TransformComponent>();
		// Check that parent has transform component, if not move to the next parent
	} while (!t);
    
    return t->getNestedMatrix(depth - 1) * getLocalMatrix();
}

glm::vec3 TransformComponent::getGlobalPosition() {
	return glm::vec3(getGlobalMatrix() * glm::vec4(0.f, 0.f, 0.f, 1.f));
}

void TransformComponent::setLocalPosition(glm::vec3 position) {
    _position = position;
}

void TransformComponent::setLocalPosition(float x, float y, float z) {
    setLocalPosition(glm::vec3(x, y, z));
}

void TransformComponent::setLocalRotation(physx::PxQuat rotation) {
      _rotation = rotation;
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
}

void TransformComponent::localTranslate(float x, float y, float z) {
	localTranslate(glm::vec3(x, y, z));
}
void TransformComponent::localRotate(physx::PxQuat rotation) {
	_rotation = rotation *_rotation;
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



ComponentEnum TransformComponent::getType() {
    return ComponentEnum::transform;
}

TransformComponent::~TransformComponent() {
    // Nothing to do here yet
}

glm::mat4 quat_to_mat4(physx::PxQuat quat)
{
	physx::PxMat44 mat4(quat);
	glm::mat4 newMat;
	newMat[0][0] = mat4[0][0];
	newMat[0][1] = mat4[0][1];
	newMat[0][2] = mat4[0][2];
	newMat[0][3] = mat4[0][3];

	newMat[1][0] = mat4[1][0];
	newMat[1][1] = mat4[1][1];
	newMat[1][2] = mat4[1][2];
	newMat[1][3] = mat4[1][3];

	newMat[2][0] = mat4[2][0];
	newMat[2][1] = mat4[2][1];
	newMat[2][2] = mat4[2][2];
	newMat[2][3] = mat4[2][3];

	newMat[3][0] = mat4[3][0];
	newMat[3][1] = mat4[3][1];
	newMat[3][2] = mat4[3][2];
	newMat[3][3] = mat4[3][3];

	//newMat = glm::scale(newMat, glm::vec3(-1.f, 1.f, 1.f));
	return newMat;//glm::transpose(newMat);
}

#include "TransformComponent.h"

TransformComponent::TransformComponent(Entity& e) 
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
	std::shared_ptr<TransformComponent> t;
	do {
		shared_ptr<Entity> parent = entity.parent();
		if (parent == nullptr) return getLocalMatrix();
		t = parent->getComponent<TransformComponent>();
		// Check that parent has transform component, if not move to the next parent
	} while (!t);
    
    return t->getNestedMatrix(depth - 1) * getLocalMatrix();
}


void TransformComponent::setLocalPosition(glm::vec3 position) {
    _position = position;
	updatePhysicsPosition();
}

void TransformComponent::setLocalPosition(physx::PxVec3 position) {
    setLocalPosition(position.x, position.y, position.z);
}

void TransformComponent::setLocalPosition(float x, float y, float z) {
    setLocalPosition(glm::vec3(x, y, z));
}


void TransformComponent::setLocalRotation(physx::PxQuat rotation) {
      _rotation = rotation;
	  updatePhysicsPosition();
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
	updatePhysicsPosition();
}

void TransformComponent::localTranslate(float x, float y, float z) {
	localTranslate(glm::vec3(x, y, z));
}


void TransformComponent::localRotate(physx::PxQuat rotation) {
	_rotation = rotation *_rotation;
	updatePhysicsPosition();
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


void TransformComponent::updatePhysicsPosition() {
	if (auto vc = entity.getComponent<VehicleComponent>()) {
		vc->setTransform(toPxTransform(getGlobalMatrix()));
	}
}


physx::PxTransform toPxTransform(glm::mat4& glm) {
	
	physx::PxMat44 px;
	
	px[0][0] = glm[0][0];
	px[0][1] = glm[0][1];
	px[0][2] = glm[0][2];
	px[0][3] = glm[0][3];

	px[1][0] = glm[1][0];
	px[1][1] = glm[1][1];
	px[1][2] = glm[1][2];
	px[1][3] = glm[1][3];

	px[2][0] = glm[2][0];
	px[2][1] = glm[2][1];
	px[2][2] = glm[2][2];
	px[2][3] = glm[2][3];

	px[3][0] = glm[3][0];
	px[3][1] = glm[3][1];
	px[3][2] = glm[3][2];
	px[3][3] = glm[3][3];

	return physx::PxTransform(px);
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

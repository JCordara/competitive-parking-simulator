#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Components.h"
#include <PhysX/PxPhysicsAPI.h>

#define TO_ROOT -1

class TransformComponent : public BaseComponent {
public:
    
    TransformComponent(shared_ptr<Entity> entity);
    static  ComponentEnum getType();
    ~TransformComponent();

    glm::vec3 getGlobalPosition();
	glm::mat4 getGlobalMatrix() { return getNestedMatrix(TO_ROOT); }
	glm::mat4 getNestedMatrix(int depth);

	glm::mat4 onlyPositionTransformGlobal();
	glm::mat4 onlyPositionTransformMatrix(int depth);

    glm::vec3     getLocalPosition() { return _position; }
	physx::PxQuat getLocalRotation() { return _rotation; }
    glm::vec3     getLocalScale()    { return _scale; }
    
    void setLocalPosition(physx::PxVec3 position);
    void setLocalPosition(glm::vec3 position);
    void setLocalPosition(float x, float y, float z);

    void setLocalRotation(physx::PxQuat rotation);
	void setLocalRotation(float rotation, physx::PxVec3 axis);
	void setLocalRotation(float rotation, glm::vec3 axis);

    void setLocalScale(glm::vec3 scale);
    void setLocalScale(float x, float y, float z);

    void localTranslate(glm::vec3 translation);
    void localTranslate(float x, float y, float z);

	void localRotate(physx::PxQuat rotation);
    void localRotate(float rotation, physx::PxVec3 axis);
	void localRotate(float rotation, glm::vec3 axis);

    void localScale(glm::vec3 scale);
    void localScale(float x, float y, float z);

	glm::mat4 getLocalTranslationMatrix();
	glm::mat4 getLocalRotationMatrix();
	glm::mat4 getLocalScaleMatrix();
	glm::mat4 getLocalMatrix();

private:
    glm::vec3 _position;
	physx::PxQuat _rotation;
    glm::vec3 _scale;

    void updateComponents();
};


#endif // TRANSFORM_COMPONENT_H

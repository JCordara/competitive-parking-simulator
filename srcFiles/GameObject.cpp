#pragma once
#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>


GameObject::GameObject(int modelID, int textureID, glm::mat4 transformation) : modelID(modelID), textureID(textureID), transformation(transformation){}

void prepareGameObjectsForRendering(GLint modelTransformationLocation, GLint inverseModelTransformationLocation, std::vector<GameObject>& gameObjects) {
	std::vector<GLfloat> ret1;
	std::vector<GLfloat> ret2;
	glm::mat4 temp;
	for (int i = 0; i < gameObjects.size(); i++) {
		temp = gameObjects[i].getTransformation();
		copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
		temp = glm::inverse(temp);
		copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret2));
	}
	glUniformMatrix4fv(modelTransformationLocation, gameObjects.size(), GL_FALSE, ret1.data());
	glUniformMatrix4fv(inverseModelTransformationLocation, gameObjects.size(), GL_TRUE, ret2.data());
}

#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iterator>

class GameObject {
public:
	GameObject(int modelID, int textureID, glm::mat4 transformation);

	void setModelID(int ID) { modelID = ID; }
	void setTextureID(int ID) { textureID = ID; }
	void setTransformation(glm::mat4 tran) { transformation = tran; }

	int getModelID() { return modelID; }
	int getTextureID() { return textureID; }
	glm::mat4 getTransformation() { return transformation; }
	glm::mat4 getInverseTransformation() { return glm::inverse(transformation); }

private:
	int modelID;
	int textureID;
	glm::mat4 transformation; //Unsure how PhysX will handle this, for now, we will bake it.

};

void prepareGameObjectsForRendering(GLint modelTransformationLocation, GLint inverseModelTransformationLocation, std::vector<GameObject>& gameObjects);

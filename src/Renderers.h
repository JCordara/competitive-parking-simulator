#pragma once
#include "Texture.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "GameObject.h"
#include "Geometry.h"
#include "Lighting.h"


struct instancedPair {
	std::shared_ptr<Model> model;
	std::vector<glm::mat4> modelTransformations;
};

class DepthRenderer {
public:
	DepthRenderer();
	void use(int width, int height);
	void setCameraTransformations(glm::mat4 V, glm::mat4 P);
	void attachFrameBufferTexture(GLuint depthTextureID);
	void render(instancedPair& instancedRender);
	void endUse();

private:
	ShaderProgram shader;
	FrameBuffer frameBuffer;
	GLint modelsLocation;
	GLint viewLocation;
	GLint projectionLocation;
};

class DeferredRenderer {
public:
	DeferredRenderer();

	void use(int width, int height);
	void setCameraTransformations(glm::mat4 V, glm::mat4 P);
	void setDirectionalLightCameraTransformations(glm::mat4 V, glm::mat4 P);
	void attachFrameBufferTextures(GLuint textureColourID, GLint textureClassificationID,
		GLint texturePositionID, GLint textureNormalID, GLint textureDiffuseConstantID,
		GLint textureSpecularAndShinnyConstantID, GLint textureAmbientConstantID,
		GLint textureDirectionalLightPositionID,GLint depthTextureID);
	void render(instancedPair& instancedRender);
	void endUse();
private:
	ShaderProgram shader;
	FrameBuffer frameBuffer;
	//Model Properties
	GLint modelTextureLocation;
	GLint modelClassificationColourLocation;
	GLint modelDiffuseConstantLocation;
	GLint modelSpecularConstantLocation;
	GLint modelAlphaConstantLocation;
	GLint modelAmbientConstantLocation;
	//Transformations
	GLint modelTransformationsLocation;
	GLint modelTransformationsInverseLocation;
	GLint cameraViewTransformationLocation;
	GLint cameraProjectionTransformationLocation;
	GLint directionalLightCameraViewTransformationLocation;
	GLint directionalLightCameraProjectionTransformationLocation;
	//Texture output locations
	GLint textureColourLocation;
	GLint textureClassificationLocation;
	GLint texturePositionLocation;
	GLint textureNormalLocation;
	GLint textureDiffuseConstantLocation;
	GLint textureSpecularAndShinnyConstantLocation;
	GLint textureAmbientConstantLocation;
	GLint textureDirectionalLightPositionLocation;
};

class PostProcessingRenderer {
public:
	PostProcessingRenderer();

	void use(int width, int height);
	void setTextureLocations(int textureColourActiveLocation, int textureClassificationActiveLocation,
		int texturePositionActiveLocation, int textureNormalActiveLocation,
		int textureDiffuseConstantActiveLocation, int textureSpecularAndShinnyConstantActiveLocation,
		int textureAmbientConstantActiveLocation, int textureDirectionalLightDepthActiveLocation,
		int textureDirectionalLightPositionActiveLocation);
	void setPointLights(std::vector<std::shared_ptr<PointLight>>& pointLights, std::vector<glm::mat4> transforms);
	void setSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotLights, std::vector<glm::mat4> transforms);
	void setDirectionalLight(std::shared_ptr<DirectionalLight> light, glm::vec3 direction);
	void setAmbientLight(std::shared_ptr<AmbientLight> light);
	void setRenderedCameraPosition(glm::vec3& pos);
	void render();
private:
	//Program to run Shader
	ShaderProgram shader;
	//Simple draw geom
	glm::vec3 quad[6] = {
		glm::vec3(1.f, 1.f, 0.f),
		glm::vec3(-1.f, 1.f, 0.f),
		glm::vec3(-1.f, -1.f, 0.f),
		glm::vec3(1.f, 1.f, 0.f),
		glm::vec3(-1.f, -1.f, 0.f),
		glm::vec3(1.f, -1.f, 0.f)
	};
	glm::vec2 quadUV[6] = {
		glm::vec2(1.f, 1.f),
		glm::vec2(0.f, 1.f),
		glm::vec2(0.f, 0.f),
		glm::vec2(1.f, 1.f),
		glm::vec2(0.f, 0.f),
		glm::vec2(1.f, 0.f)
	};
	//Vertex Array with the vertBuffer and textureUVBuffer
	VertexArray vao;
	VertexBuffer vertBuffer;
	VertexBuffer uvBuffer;
	// All textures to read from for defered shading
	GLint textureColourLocation;
	GLint textureClassificationLocation;
	GLint texturePositionLocation;
	GLint textureNormalLocation;
	GLint textureDiffuseConstantLocation;
	GLint textureSpecularAndShinnyConstantLocation;
	GLint textureAmbientConstantLocation;
	//Depth test texture of directional light Shading
	GLint textureDirectionalLightDepthLocation;
	GLint textureDirectionalLightPositionLocation;
	//Point Lighting Uniforms
	GLint pointLightPositionsLocation;
	GLint pointLightColoursLocation;
	GLint pointLightAttenuationConstaintsLocation;
	GLint pointLightRadiusLocation;
	GLint numberOfPointLightsLocation;
	//Spot Lighting Uniforms
	GLint spotLightPositionsLocation;
	GLint spotLightColoursLocation;
	GLint spotLightAttenuationConstaintsLocation;
	GLint spotLightRadiusLocation;
	GLint spotLightDirectionsLocation;
	GLint spotLightCosAnglesLocation;
	GLint numberOfSpotLightsLocation;
	//Directional Lighting Uniforms
	GLint directionalLightDirectionLocation;
	GLint directionalLightColourLocation;
	//Ambient Lighting Uniforms
	GLint ambientLightColourLocation;
	//Camera Uniforms
	GLint cameraPositionLocation;
};

class GameRenderPipeline {
public:

	GameRenderPipeline();

	//Set the lighting properties of the scene
	void addPointLight(std::shared_ptr<PointLight> pointLight, glm::mat4 transform);
	void addSpotLight(std::shared_ptr<SpotLight> spotLight, glm::mat4 transform);
	void setDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight);
	void setAmbientLight(std::shared_ptr<AmbientLight> ambientLight);
	
	//Set render properties
	void setDirectionalLightShadowMapProperties(glm::vec3 direction, glm::mat4 V, glm::mat4 P, int width, int height);
	void setCamera(glm::vec3 pos, glm::mat4 V, glm::mat4 P);
	void setWindowDimentions(int width, int height);
	//Attach Objects to render
	void attachRender(std::shared_ptr<Model> model, glm::mat4 modelTransformation);
	//Render the output
	void executeRender();
	//Clear functions
	void clearPointLights() { pointLights.clear(); }
	void clearSpotLights() { spotLights.clear(); }
	void clearRenderQueue() { renderQueue.clear(); }
	void flushLists() {
		clearPointLights();
		clearSpotLights();
		clearRenderQueue();
	}


private:
	//All of our renderers we will use
	DepthRenderer depthRenderer;
	DeferredRenderer deferredRenderer;
	PostProcessingRenderer postProcessingRenderer;
	//Render textures (framebuffers)
	Texture directionalLightDepthTexture;
	Texture textureColour;
	Texture textureClassification;
	Texture texturePosition;
	Texture textureNormal;
	Texture textureDiffuseConstant;
	Texture textureSpecularAndShinnyConstant;
	Texture textureAmbientConstant;
	Texture textureDirectionalLightPosition;
	Texture renderdDepthTexture;
	//Lighting
	std::vector<std::shared_ptr<PointLight>> pointLights;
	std::vector<glm::mat4> pointLightTransforms;
	std::vector<std::shared_ptr<SpotLight>> spotLights;
	std::vector<glm::mat4> spotLightTransforms;
	std::shared_ptr<DirectionalLight> directionalLight;
	std::shared_ptr<AmbientLight> ambientLight;
	//Directional Light render properties
	glm::vec3 directionalLightDirection;
	glm::mat4 directionalLightViewTransformation;
	glm::mat4 directionalLightProjectionTransformation;
	int directionalLightCameraWidth;
	int directionalLightCameraHeight;
	// RenderProperties
	glm::vec3 cameraPosition;
	glm::mat4 cameraViewTransformation;
	glm::mat4 cameraProjectionTransformation;
	int cameraWidth;
	int cameraHeight;
	//Object Queue
	std::map<Model*, instancedPair> renderQueue;

};

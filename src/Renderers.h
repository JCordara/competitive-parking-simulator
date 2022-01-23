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

class OrthographicDepthRenderer {
public:
	OrthographicDepthRenderer(unsigned int SHADOW_WIDTH, unsigned int SHADOW_HEIGHT);
	void renderAll(Camera& directionalLightCamera, std::vector<std::vector<GameObject>>& GameObjects, std::vector<Model>& models);
	void render(std::vector<GameObject>& GameObjects, Model& model);
	void use(Camera& directionalLightCamera);
	void endUse();
	void bindTextureForUse() {depthTexture.bind();}
	void unbindTextureForUse() {depthTexture.unbind();}
private:
	FrameBuffer depthFrameBuffer;
	Texture depthTexture;
	ShaderProgram depthTextureShader;
	unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
	GLint modelsLocation, viewLocation, projectionLocation;
};

class MainRenderer {
public:
	MainRenderer();
	void renderAll(std::vector<std::vector<GameObject>>& GameObjects, std::vector<Model>& models);
	void render(std::vector<GameObject>& GameObjects, Model& model);
	void setCameraTransformations(glm::vec3 position, glm::mat4 V, glm::mat4 P);
	void setPointLights(std::vector<PointLight>& lights);
	void setDirectionalLight(DirectionalLight& light);
	void setAmbientLight(glm::vec3& light);
	void useShadowMappingOnDirectionalLight(Camera& orthographicCamera);
	void disableShadowMappingOnDirectionalLight();
	void use();
	void endUse() {}

private:
	ShaderProgram shader;
	GLint modelsLocation, modelsInverLocation, viewLocation, projectionLocation;
	GLint useColourLocation, colourTextLocation;
	GLint lightPositionsLocation, lightColoursLocation, lightAttenuationConstaintsLocation, lightRadiusLocation, numberOfLightsLocation;
	GLint directionalLightDirectionLocation, directionalLightColourLocation, shadowMapLocation, useShadowMapLocation, lightSpaceMatrixLocation;
	GLint ambientLightLocation;
	GLint uniformPhongConstaintsLocation;
	GLint renderCameraPositionLocation;
};

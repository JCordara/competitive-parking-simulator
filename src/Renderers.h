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
	void render(std::vector<GameObject>& GameObjects, Model& model);
	void use(Camera& directionalLightCamera);
	void endUse();
	void bindTextureForUse() {glActiveTexture(GL_TEXTURE0 + 1);depthTexture.bind();}
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
	void render(std::vector<GameObject>& GameObjects, Model& model);
	void setCameraTransformations(glm::vec3 position, glm::mat4 V, glm::mat4 P);
	void setPointLights(std::vector<PointLight>& lights);
	void setSpotLights(std::vector<SpotLight>& lights);
	void setDirectionalLight(DirectionalLight& light);
	void setAmbientLight(glm::vec3& light);
	void useShadowMappingOnDirectionalLight(Camera& orthographicCamera);
	void disableShadowMappingOnDirectionalLight();
	void use(int width, int height);
	void endUse() { renderFrameBuffer.unbind(); }


	void bindTextureForUse() {
		glActiveTexture(GL_TEXTURE0);
		renderTexture.bind();
		glActiveTexture(GL_TEXTURE0 + 1);
		renderTexture_unshaded.bind();
	}
	void unbindTextureForUse() { renderTexture.unbind(); renderTexture_unshaded.unbind();
	}

private:
	ShaderProgram shader;
	GLint modelsLocation, modelsInverLocation, viewLocation, projectionLocation;
	GLint useColourLocation, colourTextLocation;
	GLint lightPositionsLocation, lightColoursLocation, lightAttenuationConstaintsLocation, lightRadiusLocation, numberOfLightsLocation;
	GLint spotLightPositionsLocation, spotLightColoursLocation, spotLightAttenuationConstaintsLocation, spotLightRadiusLocation, spotLightDirectionsLocation, spotLightCosInnerAngleLocation, spotLightCosOuterAngleLocation, numberOfSpotLightsLocation;
	GLint directionalLightDirectionLocation, directionalLightColourLocation, shadowMapLocation, useShadowMapLocation, lightSpaceMatrixLocation;
	GLint ambientLightLocation;
	GLint uniformPhongConstaintsLocation;
	GLint renderCameraPositionLocation;

	FrameBuffer renderFrameBuffer;
	Texture renderTexture, renderTexture_unshaded, depthTexture;

};

class PostProcessingRenderer {
public:
	PostProcessingRenderer() : shader("shaders/EdgeDetection.vert", "shaders/EdgeDetection.frag"), vao(), vertBuffer(0, 3, GL_FLOAT), uvBuffer(1,2,GL_FLOAT){
		tex2Location = glGetUniformLocation(shader, "in_tex2");
		tex2_unshadedLocation = glGetUniformLocation(shader, "in_tex2_no_shading");
		vao.bind();
		vertBuffer.uploadData(sizeof(glm::vec3) * 6, quad, GL_STATIC_DRAW);
		uvBuffer.uploadData(sizeof(glm::vec2) * 6, quadUV, GL_STATIC_DRAW);
		vao.unbind();
	};
	void use(int width, int height) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		shader.use();
		glUniform1i(tex2Location, 0);
		glUniform1i(tex2_unshadedLocation, 1);
	}
	void render() {
		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		vao.unbind();
	}
private:
	ShaderProgram shader;
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
	VertexArray vao;
	VertexBuffer vertBuffer;
	VertexBuffer uvBuffer;
	GLint tex2Location, tex2_unshadedLocation;
};

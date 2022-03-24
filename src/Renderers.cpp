#include "Renderers.h"


GameRenderPipeline::GameRenderPipeline() :
	depthRenderer(),
	deferredRenderer(),
	postProcessingRenderer(),
	directionalLightDepthTexture(),
	textureColour(),
	textureClassification(),
	texturePosition(),
	textureNormal(),
	textureDiffuseConstant(),
	textureSpecularAndShinnyConstant(),
	textureAmbientConstant(),
	textureDirectionalLightPosition()
{
	depthRenderer.attachFrameBufferTexture(directionalLightDepthTexture.getTextureHandleID());
	deferredRenderer.attachFrameBufferTextures(
		textureColour.getTextureHandleID(),
		textureClassification.getTextureHandleID(),
		texturePosition.getTextureHandleID(),
		textureNormal.getTextureHandleID(),
		textureDiffuseConstant.getTextureHandleID(),
		textureSpecularAndShinnyConstant.getTextureHandleID(),
		textureAmbientConstant.getTextureHandleID(),
		textureDirectionalLightPosition.getTextureHandleID(),
		renderdDepthTexture.getTextureHandleID()
	);
}


void GameRenderPipeline::addPointLight(std::shared_ptr<PointLight> pointLight, glm::mat4 transform) {
	pointLights.push_back(pointLight);
	pointLightTransforms.push_back(transform);
}

void GameRenderPipeline::addSpotLight(std::shared_ptr<SpotLight> spotLight, glm::mat4 transform) {
	spotLights.push_back(spotLight);
	spotLightTransforms.push_back(transform);
}

void GameRenderPipeline::setDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight) {
	this->directionalLight = directionalLight;
}
void GameRenderPipeline::setAmbientLight(std::shared_ptr<AmbientLight> ambientLight) {
	this->ambientLight = ambientLight;
}

//Set render properties
void GameRenderPipeline::setDirectionalLightShadowMapProperties(glm::vec3 direction, glm::mat4 V, glm::mat4 P, int width, int height) {
	directionalLightViewTransformation = V;
	directionalLightProjectionTransformation = P;
	directionalLightDirection = direction;
	if (directionalLightCameraWidth != width || directionalLightCameraHeight != height) {
		directionalLightCameraWidth = width;
		directionalLightCameraHeight = height;
		directionalLightDepthTexture.setUpInternal(directionalLightCameraWidth, directionalLightCameraHeight, GL_NEAREST, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	}
}

void GameRenderPipeline::setCamera(glm::vec3 pos, glm::mat4 V, glm::mat4 P, float nearClip, float farClip) {
	cameraPosition = pos;
	cameraViewTransformation = V;
	cameraProjectionTransformation = P;
	this->nearClip = nearClip;
	this->farClip = farClip;
}
void GameRenderPipeline::setWindowDimentions(int width, int height) {
	if (cameraWidth != width || cameraHeight != height) {
		cameraWidth = width;
		cameraHeight = height;
		textureColour.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
		textureClassification.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
		texturePosition.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB16F, GL_RGB, GL_FLOAT);
		textureNormal.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB16F, GL_RGB, GL_FLOAT);
		textureDiffuseConstant.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB16F, GL_RGB, GL_FLOAT);
		textureSpecularAndShinnyConstant.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		textureAmbientConstant.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB16F, GL_RGB, GL_FLOAT);
		textureDirectionalLightPosition.setUpInternal(cameraWidth, cameraHeight, GL_LINEAR, GL_RGB16F, GL_RGB, GL_FLOAT);
		renderdDepthTexture.setUpInternal(cameraWidth, cameraHeight, GL_NEAREST, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	}
}

void GameRenderPipeline::attachRender(std::shared_ptr<Model> model, glm::mat4 modelTransformation) {
	auto it = renderQueue.find(model.get());
	if (it != renderQueue.end())//Already exists
		it->second.modelTransformations.push_back(modelTransformation);
	else {
		instancedPair newPair;
		newPair.model = model;
		newPair.modelTransformations.push_back(modelTransformation);
		renderQueue.insert(std::pair<Model*, instancedPair>(model.get(), newPair));
	}
}

void GameRenderPipeline::executeRender() {
	//Depth Pass
	depthRenderer.use(directionalLightCameraWidth, directionalLightCameraHeight);
	depthRenderer.setCameraTransformations(directionalLightViewTransformation, directionalLightProjectionTransformation);
	for (auto it = renderQueue.begin(); it != renderQueue.end(); it++) depthRenderer.render(it->second);
	//depthRenderer.endUse();
	//Deferred Pass
	deferredRenderer.use(cameraWidth, cameraHeight);
	deferredRenderer.setCameraTransformations(cameraViewTransformation, cameraProjectionTransformation);
	deferredRenderer.setDirectionalLightCameraTransformations(directionalLightViewTransformation, directionalLightProjectionTransformation);
	for (auto it = renderQueue.begin(); it != renderQueue.end(); it++) deferredRenderer.render(it->second);
	deferredRenderer.endUse();
	deferredRenderer.copyDepth(0, renderdDepthTexture.getDimensions().x, renderdDepthTexture.getDimensions().y);
	//Post Processing Pass
	postProcessingRenderer.use(cameraWidth, cameraHeight);
	glActiveTexture(GL_TEXTURE0);
	textureColour.bind();
	glActiveTexture(GL_TEXTURE1);
	textureClassification.bind();
	glActiveTexture(GL_TEXTURE2);
	texturePosition.bind();
	glActiveTexture(GL_TEXTURE3);
	textureNormal.bind();
	glActiveTexture(GL_TEXTURE4);
	textureDiffuseConstant.bind();
	glActiveTexture(GL_TEXTURE5);
	textureSpecularAndShinnyConstant.bind();
	glActiveTexture(GL_TEXTURE6);
	textureAmbientConstant.bind();
	glActiveTexture(GL_TEXTURE7);
	renderdDepthTexture.bind();
	glActiveTexture(GL_TEXTURE8);
	directionalLightDepthTexture.bind();
	glActiveTexture(GL_TEXTURE9);
	textureDirectionalLightPosition.bind();

	postProcessingRenderer.setTextureLocations(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	postProcessingRenderer.setPointLights(pointLights, pointLightTransforms);
	postProcessingRenderer.setSpotLights(spotLights, spotLightTransforms);
	postProcessingRenderer.setDirectionalLight(directionalLight, directionalLightDirection);
	postProcessingRenderer.setAmbientLight(ambientLight);
	postProcessingRenderer.setRenderedCameraPosition(cameraPosition);
	postProcessingRenderer.setRenderedCameraClip(nearClip, farClip);
	postProcessingRenderer.render();
	//postProcessingRenderer.endUse();
}

DepthRenderer::DepthRenderer() : shader("shaders/DepthTexture.vert", "shaders/DepthTexture.frag"), frameBuffer() {
	std::vector<GLenum> drawBuffer = { GL_NONE };
	frameBuffer.drawBuffers(drawBuffer);
	modelsLocation = glGetUniformLocation(shader, "Ms");
	viewLocation = glGetUniformLocation(shader, "V");
	projectionLocation = glGetUniformLocation(shader, "P");
}
void DepthRenderer::use(int width, int height) {
	shader.use();
	frameBuffer.bind();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DepthRenderer::setCameraTransformations(glm::mat4 V, glm::mat4 P) {
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &P[0][0]);

}

void DepthRenderer::attachFrameBufferTexture(GLuint depthTextureID) {
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	frameBuffer.attachTexture(GL_DEPTH_ATTACHMENT, depthTextureID);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DepthRenderer::render(instancedPair& instancedRender) {
	std::vector<GLfloat> ret1;
	glm::mat4 temp;
	int j = 0;
	int count = 0;
	while(j < instancedRender.modelTransformations.size()) {
		for (
			int count = 0;
			j < instancedRender.modelTransformations.size() && count < 120;
			j++, count++
		) {
			temp = instancedRender.modelTransformations[j];
			copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
		}
		glUniformMatrix4fv(modelsLocation, instancedRender.modelTransformations.size(), GL_FALSE, ret1.data());
		instancedRender.model->draw(shader, GL_TEXTURE0, -1, -1, -1, -1, -1, instancedRender.modelTransformations.size());
		ret1.clear();
	}
}

void DepthRenderer::endUse() {
	frameBuffer.unbind();
}

DeferredRenderer::DeferredRenderer() : shader("shaders/DeferredShading.vert", "shaders/DeferredShading.frag"), frameBuffer() {
	//Model Properties
	modelTextureLocation = glGetUniformLocation(shader, "colourTexture");
	modelClassificationColourLocation = glGetUniformLocation(shader, "modelColour");
	modelDiffuseConstantLocation = glGetUniformLocation(shader, "uniformPhongDiffuse");
	modelSpecularConstantLocation = glGetUniformLocation(shader, "uniformPhongSpecular");
	modelAlphaConstantLocation = glGetUniformLocation(shader, "uniformPhongAlpha");
	modelAmbientConstantLocation = glGetUniformLocation(shader, "uniformPhongAmbient");
	//Transformations
	modelTransformationsLocation = glGetUniformLocation(shader, "Ms");
	modelTransformationsInverseLocation = glGetUniformLocation(shader, "MsInverseTransposed");
	cameraViewTransformationLocation = glGetUniformLocation(shader, "cameraV");
	cameraProjectionTransformationLocation = glGetUniformLocation(shader, "cameraP");
	directionalLightCameraViewTransformationLocation = glGetUniformLocation(shader, "directionalLightCameraV");
	directionalLightCameraProjectionTransformationLocation = glGetUniformLocation(shader, "directionalLightCameraP");
	//Texture output locations
	textureColourLocation = glGetUniformLocation(shader, "textureColour");
	textureClassificationLocation = glGetUniformLocation(shader, "textureClassification");
	texturePositionLocation = glGetUniformLocation(shader, "texturePosition");
	textureNormalLocation = glGetUniformLocation(shader, "textureNormal");
	textureDiffuseConstantLocation = glGetUniformLocation(shader, "textureDiffuseConstant");
	textureSpecularAndShinnyConstantLocation = glGetUniformLocation(shader, "textureSpecularAndShinnyConstant");
	textureAmbientConstantLocation = glGetUniformLocation(shader, "textureAmbientConstant");
	textureDirectionalLightPositionLocation = glGetUniformLocation(shader, "textureDirectionalLightPosition");
	std::vector<GLenum> drawBuffer = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7
	};
	frameBuffer.drawBuffers(drawBuffer);
}

void DeferredRenderer::use(int width, int height) {
	shader.use();
	frameBuffer.bind();
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.1f, 0.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glUniform1i(modelTextureLocation, 0);
}

void DeferredRenderer::setCameraTransformations(glm::mat4 V, glm::mat4 P) {
	glUniformMatrix4fv(cameraViewTransformationLocation, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(cameraProjectionTransformationLocation, 1, GL_FALSE, &P[0][0]);
}

void DeferredRenderer::setDirectionalLightCameraTransformations(glm::mat4 V, glm::mat4 P) {
	glUniformMatrix4fv(directionalLightCameraViewTransformationLocation, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(directionalLightCameraProjectionTransformationLocation, 1, GL_FALSE, &P[0][0]);
}
void DeferredRenderer::attachFrameBufferTextures(GLuint textureColourID, GLint textureClassificationID,
	GLint texturePositionID, GLint textureNormalID, GLint textureDiffuseConstantID,
	GLint textureSpecularAndShinnyConstantID, GLint textureAmbientConstantID,
	GLint textureDirectionalLightPositionID, GLint depthTextureID) {
	glBindTexture(GL_TEXTURE_2D, textureColourID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT0, textureColourID);
	glBindTexture(GL_TEXTURE_2D, textureClassificationID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT1, textureClassificationID);
	glBindTexture(GL_TEXTURE_2D, texturePositionID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT2, texturePositionID);
	glBindTexture(GL_TEXTURE_2D, textureNormalID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT3, textureNormalID);
	glBindTexture(GL_TEXTURE_2D, textureDiffuseConstantID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT4, textureDiffuseConstantID);
	glBindTexture(GL_TEXTURE_2D, textureSpecularAndShinnyConstantID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT5, textureSpecularAndShinnyConstantID);
	glBindTexture(GL_TEXTURE_2D, textureAmbientConstantID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT6, textureAmbientConstantID);
	glBindTexture(GL_TEXTURE_2D, textureDirectionalLightPositionID);
	frameBuffer.attachTexture(GL_COLOR_ATTACHMENT7, textureDirectionalLightPositionID);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	frameBuffer.attachTexture(GL_DEPTH_ATTACHMENT, depthTextureID);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DeferredRenderer::render(instancedPair& instancedRender) {
	std::vector<GLfloat> ret1, ret2;
	glm::mat4 temp;
	int j = 0;
	int count = 0;
	while (j < instancedRender.modelTransformations.size()) {
		for (
			int count = 0;
			j < instancedRender.modelTransformations.size() && count < 120;
			j++, count++
		) {
			temp = instancedRender.modelTransformations[j];
			copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
			temp = glm::inverse(temp);
			copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret2));
		}
		glUniformMatrix4fv(modelTransformationsLocation, instancedRender.modelTransformations.size(), GL_FALSE, ret1.data());
		glUniformMatrix4fv(modelTransformationsInverseLocation, instancedRender.modelTransformations.size(), GL_TRUE, ret2.data());
		instancedRender.model->draw(shader, GL_TEXTURE0, modelClassificationColourLocation,
			modelDiffuseConstantLocation, modelSpecularConstantLocation, modelAlphaConstantLocation, modelAmbientConstantLocation, instancedRender.modelTransformations.size());
		ret1.clear();
		ret2.clear();
	}
}
void DeferredRenderer::endUse() {
	frameBuffer.unbind();
}

void DeferredRenderer::copyDepth(GLuint id, int width, int height) {
	frameBuffer.copyDepth(id, width, height);
}


PostProcessingRenderer::PostProcessingRenderer() :
	shader("shaders/PostProcessingDeferredShading.vert", "shaders/PostProcessingDeferredShading.frag"),
	vao(),
	vertBuffer(0, 3, GL_FLOAT),
	uvBuffer(1, 2, GL_FLOAT) {
	// All textures to read from for defered shading
	textureColourLocation = glGetUniformLocation(shader, "textureColour");
	textureClassificationLocation = glGetUniformLocation(shader, "textureClassification");
	texturePositionLocation = glGetUniformLocation(shader, "texturePosition");
	textureNormalLocation = glGetUniformLocation(shader, "textureNormal");
	textureDiffuseConstantLocation = glGetUniformLocation(shader, "textureDiffuseConstant");
	textureSpecularAndShinnyConstantLocation = glGetUniformLocation(shader, "textureSpecularAndShinnyConstant");
	textureSpecularAndShinnyConstantLocation = glGetUniformLocation(shader, "textureSpecularAndShinnyConstant");
	textureDepthLocation = glGetUniformLocation(shader, "textureDepth");
	//Depth test texture of directional light Shading
	textureDirectionalLightDepthLocation = glGetUniformLocation(shader, "textureDirectionalLightDepth");
	textureDirectionalLightPositionLocation = glGetUniformLocation(shader, "textureDirectionalLightPosition");
	//Point Lighting Uniforms
	pointLightPositionsLocation = glGetUniformLocation(shader, "pointLightPositions");
	pointLightColoursLocation = glGetUniformLocation(shader, "pointLightColours");
	pointLightAttenuationConstaintsLocation = glGetUniformLocation(shader, "pointLightAttenuationConstaints");
	pointLightRadiusLocation = glGetUniformLocation(shader, "pointLightRadius");
	numberOfPointLightsLocation = glGetUniformLocation(shader, "numberOfPointLights");
	//Spot Lighting Uniforms
	spotLightPositionsLocation = glGetUniformLocation(shader, "spotLightPositions");
	spotLightColoursLocation = glGetUniformLocation(shader, "spotLightColours");
	spotLightAttenuationConstaintsLocation = glGetUniformLocation(shader, "spotLightAttenuationConstaints");
	spotLightRadiusLocation = glGetUniformLocation(shader, "spotLightRadius");
	spotLightDirectionsLocation = glGetUniformLocation(shader, "spotLightDirections");
	spotLightCosAnglesLocation = glGetUniformLocation(shader, "spotLightCosAngles");
	numberOfSpotLightsLocation = glGetUniformLocation(shader, "numberOfSpotLights");
	//Directional Lighting Uniforms
	directionalLightDirectionLocation = glGetUniformLocation(shader, "directionalLightDirection");
	directionalLightColourLocation = glGetUniformLocation(shader, "directionalLightColour");
	//Ambient Lighting Uniforms
	ambientLightColourLocation = glGetUniformLocation(shader, "ambientLightColour");
	//Camera Uniforms
	cameraPositionLocation = glGetUniformLocation(shader, "cameraPosition");
	clipDistanceLocation = glGetUniformLocation(shader, "clipDistance");
	//Geom being set
	vao.bind();
	vertBuffer.uploadData(sizeof(glm::vec3) * 6, quad, GL_STATIC_DRAW);
	uvBuffer.uploadData(sizeof(glm::vec2) * 6, quadUV, GL_STATIC_DRAW);
	vao.unbind();
};

void PostProcessingRenderer::use(int width, int height) {
	//Setup render call settings
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);
	//Load the shader for use
	shader.use();
}

void PostProcessingRenderer::setTextureLocations(int textureColourActiveLocation, int textureClassificationActiveLocation,
	int texturePositionActiveLocation, int textureNormalActiveLocation,
	int textureDiffuseConstantActiveLocation, int textureSpecularAndShinnyConstantActiveLocation,
	int textureAmbientConstantActiveLocation, int textureDepthActiveLocation,
	int textureDirectionalLightDepthActiveLocation, int textureDirectionalLightPositionActiveLocation) {
	//Set the texture locations in the Active API
	glUniform1i(textureColourLocation, textureColourActiveLocation);
	glUniform1i(textureClassificationLocation, textureClassificationActiveLocation);
	glUniform1i(texturePositionLocation, texturePositionActiveLocation);
	glUniform1i(textureNormalLocation, textureNormalActiveLocation);
	glUniform1i(textureDiffuseConstantLocation, textureDiffuseConstantActiveLocation);
	glUniform1i(textureSpecularAndShinnyConstantLocation, textureSpecularAndShinnyConstantActiveLocation);
	glUniform1i(textureAmbientConstantLocation, textureAmbientConstantActiveLocation);
	glUniform1i(textureDepthLocation, textureDepthActiveLocation);
	glUniform1i(textureDirectionalLightDepthLocation, textureDirectionalLightDepthActiveLocation);
	glUniform1i(textureDirectionalLightPositionLocation, textureDirectionalLightPositionActiveLocation);
}

void PostProcessingRenderer::setPointLights(std::vector<std::shared_ptr<PointLight>>& pointLights, std::vector<glm::mat4> transforms) {
	std::vector<std::vector<GLfloat>> ret = std::vector<std::vector<GLfloat>>(4);
	ret[0] = std::vector<GLfloat>(3 * pointLights.size());
	ret[1] = std::vector<GLfloat>(3 * pointLights.size());
	ret[2] = std::vector<GLfloat>(3 * pointLights.size());
	ret[3] = std::vector<GLfloat>(pointLights.size());
	glm::vec3 temp;
	glm::vec4 temp4 = glm::vec4(0.f,0.f,0.f,1.f);
	for (int i = 0; i < pointLights.size(); i++) {
		temp = glm::vec3(transforms[i] * temp4); //pointLights[i]->getPos();
		ret[0][3 * i + 0] = temp[0];
		ret[0][3 * i + 1] = temp[1];
		ret[0][3 * i + 2] = temp[2];
		temp = pointLights[i]->getCol();
		ret[1][3 * i + 0] = temp[0];
		ret[1][3 * i + 1] = temp[1];
		ret[1][3 * i + 2] = temp[2];
		temp = pointLights[i]->getAttenuationConsts();
		ret[2][3 * i + 0] = temp[0];
		ret[2][3 * i + 1] = temp[1];
		ret[2][3 * i + 2] = temp[2];
		ret[3][i] = pointLights[i]->geRadius();
	}
	glUniform3fv(pointLightPositionsLocation, pointLights.size(), ret[0].data());
	glUniform3fv(pointLightColoursLocation, pointLights.size(), ret[1].data());
	glUniform3fv(pointLightAttenuationConstaintsLocation, pointLights.size(), ret[2].data());
	glUniform1fv(pointLightRadiusLocation, pointLights.size(), ret[3].data());
	glUniform1i(numberOfPointLightsLocation, pointLights.size());
}

void PostProcessingRenderer::setSpotLights(std::vector<std::shared_ptr<SpotLight>>& spotLights, std::vector<glm::mat4> transforms) {
	std::vector<std::vector<GLfloat>> ret = std::vector<std::vector<GLfloat>>(7);
	ret[0] = std::vector<GLfloat>(3 * spotLights.size());
	ret[1] = std::vector<GLfloat>(3 * spotLights.size());
	ret[2] = std::vector<GLfloat>(3 * spotLights.size());
	ret[3] = std::vector<GLfloat>(3 * spotLights.size());
	ret[4] = std::vector<GLfloat>(	  spotLights.size());
	ret[5] = std::vector<GLfloat>(2 * spotLights.size());
	glm::vec3 temp;
	glm::vec4 temp4 = glm::vec4(0.f, 0.f, 0.f, 1.f);
	for (int i = 0; i < spotLights.size(); i++) {
		temp = glm::vec3(transforms[i] * temp4); //spotLights[i]->getPos();
		ret[0][3 * i + 0] = temp[0];
		ret[0][3 * i + 1] = temp[1];
		ret[0][3 * i + 2] = temp[2];
		temp = spotLights[i]->getCol();
		ret[1][3 * i + 0] = temp[0];
		ret[1][3 * i + 1] = temp[1];
		ret[1][3 * i + 2] = temp[2];
		temp = spotLights[i]->getAttenuationConsts();
		ret[2][3 * i + 0] = temp[0];
		ret[2][3 * i + 1] = temp[1];
		ret[2][3 * i + 2] = temp[2];
		temp4 = glm::vec4(1.f, 0.f, 0.f, 0.f);
		temp = glm::vec3(transforms[i] * temp4);
		ret[3][3 * i + 0] = temp[0];
		ret[3][3 * i + 1] = temp[1];
		ret[3][3 * i + 2] = temp[2];
		ret[4][i] = spotLights[i]->geRadius();
		ret[5][2 * i + 0] = spotLights[i]->getCosInnerAngle();
		ret[5][2 * i + 1] = spotLights[i]->getCosOuterAngle();
	}
	glUniform3fv(spotLightPositionsLocation, spotLights.size(), ret[0].data());
	glUniform3fv(spotLightColoursLocation, spotLights.size(), ret[1].data());
	glUniform3fv(spotLightAttenuationConstaintsLocation, spotLights.size(), ret[2].data());
	glUniform3fv(spotLightDirectionsLocation, spotLights.size(), ret[3].data());
	glUniform1fv(spotLightRadiusLocation, spotLights.size(), ret[4].data());
	glUniform2fv(spotLightCosAnglesLocation, spotLights.size(), ret[5].data());
	glUniform1i(numberOfSpotLightsLocation, spotLights.size());
}

void PostProcessingRenderer::setDirectionalLight(std::shared_ptr<DirectionalLight> light, glm::vec3 direction) {
	glm::vec4 temp4 = glm::vec4(0.f, 0.f, 1.f, 0.f);
	glUniform3fv(directionalLightColourLocation, 1, &light->getCol()[0]);
	glUniform3fv(directionalLightDirectionLocation, 1, &(direction)[0]);
}

void PostProcessingRenderer::setAmbientLight(std::shared_ptr<AmbientLight> light) {
	glUniform3fv(ambientLightColourLocation, 1, &light->getCol()[0]);
}

void PostProcessingRenderer::setRenderedCameraPosition(glm::vec3& pos) {
	glUniform3fv(cameraPositionLocation, 1, &pos[0]);
}
void PostProcessingRenderer::setRenderedCameraClip(float near, float far) {
	glUniform2f(clipDistanceLocation, near, far);
}

void PostProcessingRenderer::render() {
	//Draw quad
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	vao.unbind();
}

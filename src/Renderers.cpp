#include "Renderers.h"


OrthographicDepthRenderer::OrthographicDepthRenderer(unsigned int SHADOW_WIDTH, unsigned int SHADOW_HEIGHT) :
	SHADOW_WIDTH(SHADOW_WIDTH), SHADOW_HEIGHT(SHADOW_HEIGHT) {
	depthTextureShader = ShaderProgram("shaders/DepthTexture.vert", "shaders/DepthTexture.frag");
	depthTexture = Texture(SHADOW_WIDTH, SHADOW_HEIGHT, GL_NEAREST, GL_DEPTH_COMPONENT, GL_FLOAT);
	depthTexture.setBorderColour(glm::vec4(1.f, 1.f, 1.f, 1.f));
	depthFrameBuffer.attachTexture(GL_DEPTH_ATTACHMENT, depthTexture.getTextureHandleID(), GL_NONE);
	modelsLocation = glGetUniformLocation(depthTextureShader, "Ms");
	viewLocation = glGetUniformLocation(depthTextureShader, "V");
	projectionLocation = glGetUniformLocation(depthTextureShader, "P");
}
void OrthographicDepthRenderer::use(Camera& directionalLightCamera) {
	depthFrameBuffer.bind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_FRONT);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	depthTextureShader.use();
	directionalLightCamera.useOrthographic(viewLocation, projectionLocation);
}

void OrthographicDepthRenderer::render(std::vector<GameObject>& GameObjects, Model& model) {
	std::vector<GLfloat> ret1;
	glm::mat4 temp;
	GPU_Geometry drawGeometry; //TODO: we are creating the buffers every time, need to figure out why it doesnt work if we have it as a member variable
	ret1.clear();
	if (GameObjects.size() > 0) {
		for (int j = 0; j < GameObjects.size(); j++) {
			temp = GameObjects[j].getTransformation();
			copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
		}
		glUniformMatrix4fv(modelsLocation, GameObjects.size(), GL_FALSE, ret1.data());
		CPU_Geometry geom = model.getCPU_Geometry();
		drawGeometry.setVerts(geom.verts);
		drawGeometry.setCols(geom.cols);
		drawGeometry.setNormals(geom.normals);
		drawGeometry.setTexCoords(geom.texCoords);
		drawGeometry.setInds(geom.ind);
		drawGeometry.bind();
		glDrawElementsInstanced(GL_TRIANGLES, geom.ind.size(), GL_UNSIGNED_INT, (void*)0, GameObjects.size());
	}
}


void OrthographicDepthRenderer::endUse() {
	depthFrameBuffer.unbind();
}

void OrthographicDepthRenderer::renderAll(Camera& directionalLightCamera, std::vector<std::vector<GameObject>>& GameObjects, std::vector<Model>& models) {
	use(directionalLightCamera);

	std::vector<GLfloat> ret1;
	glm::mat4 temp;
	GPU_Geometry drawGeometry;
	for (int i = 0; i < GameObjects.size(); i++) {
		ret1.clear();
		if (GameObjects[i].size() > 0) {
			int ID = GameObjects[i][0].getModelID();
			for (int j = 0; j < GameObjects[i].size(); j++) {
				temp = GameObjects[i][j].getTransformation();
				copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
			}
			glUniformMatrix4fv(modelsLocation, GameObjects[i].size(), GL_FALSE, ret1.data());
			CPU_Geometry geom = models[ID].getCPU_Geometry();
			drawGeometry.setVerts(geom.verts);
			drawGeometry.setCols(geom.cols);
			drawGeometry.setNormals(geom.normals);
			drawGeometry.setTexCoords(geom.texCoords);
			drawGeometry.setInds(geom.ind);
			drawGeometry.bind();
			glDrawElementsInstanced(GL_TRIANGLES, geom.ind.size(), GL_UNSIGNED_INT, (void*)0, GameObjects[i].size());
		}
	}
	endUse();
}

MainRenderer::MainRenderer(): shader("shaders/MainCamera.vert", "shaders/MainCamera.frag") {
	modelsLocation = glGetUniformLocation(shader, "Ms");
	modelsInverLocation = glGetUniformLocation(shader, "MsInverseTransposed");
	viewLocation = glGetUniformLocation(shader, "V");
	projectionLocation = glGetUniformLocation(shader, "P");
	useColourLocation = glGetUniformLocation(shader, "useColour");
	//colourTextLocation
	lightPositionsLocation = glGetUniformLocation(shader, "lightPositions");
	lightColoursLocation = glGetUniformLocation(shader, "lightColours");
	lightAttenuationConstaintsLocation = glGetUniformLocation(shader, "lightAttenuationConstaints");
	lightRadiusLocation = glGetUniformLocation(shader, "lightRadius");
	numberOfLightsLocation = glGetUniformLocation(shader, "numberOfLights");
	directionalLightDirectionLocation = glGetUniformLocation(shader, "directionalLightDirection");
	directionalLightColourLocation = glGetUniformLocation(shader, "directionalLightColour");
	//shadowMapLocation
	useShadowMapLocation = glGetUniformLocation(shader, "useShadowMap");
	lightSpaceMatrixLocation = glGetUniformLocation(shader, "lightSpaceMatrix");
	ambientLightLocation = glGetUniformLocation(shader, "ambientLight");
	uniformPhongConstaintsLocation = glGetUniformLocation(shader, "uniformPhongConstaints");
	renderCameraPositionLocation = glGetUniformLocation(shader, "renderCameraPosition");
}
void MainRenderer::renderAll(std::vector<std::vector<GameObject>>& GameObjects, std::vector<Model>& models) {
	std::vector<GLfloat> ret1, ret2;
	glm::mat4 temp;
	GPU_Geometry drawGeometry;
	for (int i = 0; i < GameObjects.size(); i++) {
		ret1.clear();
		if (GameObjects[i].size() > 0) {
			int ID = GameObjects[i][0].getModelID();
			for (int j = 0; j < GameObjects[i].size(); j++) {
				temp = GameObjects[i][j].getTransformation();
				copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
				temp = glm::inverse(temp);
				copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret2));
			}
			glUniformMatrix4fv(modelsLocation, GameObjects[i].size(), GL_FALSE, ret1.data());
			glUniformMatrix4fv(modelsInverLocation, GameObjects[i].size(), GL_TRUE, ret2.data());
			glUniform1i(useColourLocation, (models[ID].isColoured()) ? 1 : 0);
			glUniform4fv(uniformPhongConstaintsLocation, 1, &models[ID].getPhongConstants()[0]);
			CPU_Geometry geom = models[ID].getCPU_Geometry();
			drawGeometry.setVerts(geom.verts);
			drawGeometry.setCols(geom.cols);
			drawGeometry.setNormals(geom.normals);
			drawGeometry.setTexCoords(geom.texCoords);
			drawGeometry.setInds(geom.ind);
			drawGeometry.bind();
			glDrawElementsInstanced(GL_TRIANGLES, geom.ind.size(), GL_UNSIGNED_INT, (void*)0, GameObjects[i].size());
		}
	}
}
void MainRenderer::render(std::vector<GameObject>& GameObjects, Model& model) {
	std::vector<GLfloat> ret1, ret2;
	glm::mat4 temp;
	GPU_Geometry drawGeometry;
	if (GameObjects.size() > 0) {
		for (int j = 0; j < GameObjects.size(); j++) {
				temp = GameObjects[j].getTransformation();
				copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret1));
				temp = glm::inverse(temp);
				copy(glm::value_ptr(temp), glm::value_ptr(temp) + 16, back_inserter(ret2));
		}
		glUniformMatrix4fv(modelsLocation, GameObjects.size(), GL_FALSE, ret1.data());
		glUniformMatrix4fv(modelsInverLocation, GameObjects.size(), GL_TRUE, ret2.data());
		glUniform1i(useColourLocation, (model.isColoured()) ? 1 : 0);
		glUniform4fv(uniformPhongConstaintsLocation, 1, &model.getPhongConstants()[0]);
		CPU_Geometry geom = model.getCPU_Geometry();
		drawGeometry.setVerts(geom.verts);
		drawGeometry.setCols(geom.cols);
		drawGeometry.setNormals(geom.normals);
		drawGeometry.setTexCoords(geom.texCoords);
		drawGeometry.setInds(geom.ind);
		drawGeometry.bind();
		glDrawElementsInstanced(GL_TRIANGLES, geom.ind.size(), GL_UNSIGNED_INT, (void*)0, GameObjects.size());
	}
}


void MainRenderer::setCameraTransformations(glm::vec3 position, glm::mat4 V, glm::mat4 P) {
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(P));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(V));
	glUniform3fv(renderCameraPositionLocation, 1, glm::value_ptr(position));
}
void MainRenderer::setPointLights(std::vector<PointLight>& pointLights) {
	std::vector<std::vector<GLfloat>> ret = std::vector<std::vector<GLfloat>>(4);
	ret[0] = std::vector<GLfloat>(3 * pointLights.size());
	ret[1] = std::vector<GLfloat>(3 * pointLights.size());
	ret[2] = std::vector<GLfloat>(3 * pointLights.size());
	ret[3] = std::vector<GLfloat>(pointLights.size());
	glm::vec3 temp;
	for (int i = 0; i < pointLights.size(); i++) {
		temp = pointLights[i].getPos();
		ret[0][3 * i + 0] = temp[0];
		ret[0][3 * i + 1] = temp[1];
		ret[0][3 * i + 2] = temp[2];
		temp = pointLights[i].getCol();
		ret[1][3 * i + 0] = temp[0];
		ret[1][3 * i + 1] = temp[1];
		ret[1][3 * i + 2] = temp[2];
		temp = pointLights[i].getAttenuationConsts();
		ret[2][3 * i + 0] = temp[0];
		ret[2][3 * i + 1] = temp[1];
		ret[2][3 * i + 2] = temp[2];
		ret[3][i] = pointLights[i].geRadius();
	}
	glUniform3fv(lightPositionsLocation, pointLights.size(), ret[0].data());
	glUniform3fv(lightColoursLocation, pointLights.size(), ret[1].data());
	glUniform3fv(lightAttenuationConstaintsLocation, pointLights.size(), ret[2].data());
	glUniform1fv(lightRadiusLocation, pointLights.size(), ret[3].data());
	glUniform1i(numberOfLightsLocation, pointLights.size());
}

void MainRenderer::setDirectionalLight(DirectionalLight& light) {
	glUniform3fv(directionalLightColourLocation, 1, &light.getCol()[0]);
	glUniform3fv(directionalLightDirectionLocation, 1, &light.getDirection()[0]);
}
void MainRenderer::setAmbientLight(glm::vec3& light) {
	glUniform3fv(ambientLightLocation, 1, &light[0]);
}
void MainRenderer::useShadowMappingOnDirectionalLight(Camera& orthographicCamera) {
	glm::mat4 lightSpace = orthographicCamera.getOrthographicProjection() * orthographicCamera.getView();
	glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &lightSpace[0][0]);
	glUniform1i(useShadowMapLocation, 1);
	
}
void MainRenderer::disableShadowMappingOnDirectionalLight() {
	glUniform1i(useShadowMapLocation, 0);
}

void MainRenderer::use() {
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shader.use();
}

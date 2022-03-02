#include "LightingComponent.h"


LightingComponent::LightingComponent(Entity& parent)
	: BaseComponent(parent)
{}

ComponentEnum LightingComponent::getType() {
	return ComponentEnum::lighting;
}

LightingComponent::~LightingComponent() {
	// Nothing to do here yet
}

void LightingComponent::setPointLight(glm::vec3 col, glm::vec3 attenuationConsts) {
	pointLight = make_shared<PointLight>(col, attenuationConsts);
}
void LightingComponent::setSpotLight(glm::vec3 col, glm::vec3 attenuationConsts, glm::vec3 direction, float innerAngle, float outerAngle) {
	spotLight = make_shared<SpotLight>(col, attenuationConsts, direction, innerAngle, outerAngle);
}
void LightingComponent::setDirectionalLight(glm::vec3 col, glm::vec3 direction) {
	directionalLight = make_shared<DirectionalLight>(col, direction);
}
void LightingComponent::setAmbient(glm::vec3 col) {
	ambientLight = make_shared<AmbientLight>(col);
}

std::shared_ptr<PointLight> LightingComponent::getPointLight() { return pointLight; }
std::shared_ptr<SpotLight> LightingComponent::getSpotLight() { return spotLight; }
std::shared_ptr<DirectionalLight> LightingComponent::getDirectionalLight() { return directionalLight; }
std::shared_ptr<AmbientLight> LightingComponent::getAmbient() { return ambientLight; }

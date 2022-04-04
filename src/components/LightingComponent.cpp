#include "LightingComponent.h"


LightingComponent::LightingComponent(weak_ptr<Entity> parent)
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
void LightingComponent::setSpotLight(glm::vec3 col, glm::vec3 attenuationConsts, float innerAngle, float outerAngle) {
	spotLight = make_shared<SpotLight>(col, attenuationConsts, innerAngle, outerAngle);
}
void LightingComponent::setDirectionalLight(glm::vec3 col) {
	directionalLight = make_shared<DirectionalLight>(col);
}
void LightingComponent::setAmbient(glm::vec3 col) {
	ambientLight = make_shared<AmbientLight>(col);
}

std::shared_ptr<PointLight> LightingComponent::getPointLight() { return pointLight; }
std::shared_ptr<SpotLight> LightingComponent::getSpotLight() { return spotLight; }
std::shared_ptr<DirectionalLight> LightingComponent::getDirectionalLight() { return directionalLight; }
std::shared_ptr<AmbientLight> LightingComponent::getAmbient() { return ambientLight; }

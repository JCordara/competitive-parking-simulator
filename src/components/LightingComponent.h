#ifndef LIGHTING_COMPONENT_H
#define LIGHTING_COMPONENT_H

#include "Components.h"
#include <Lighting.h>

class LightingComponent : public BaseComponent {
public:
	LightingComponent(weak_ptr<Entity> parent);

	void setPointLight(glm::vec3 col, glm::vec3 attenuationConsts);
	void setSpotLight(glm::vec3 col, glm::vec3 attenuationConsts, float innerAngle, float outerAngle);
	void setDirectionalLight(glm::vec3 col);
	void setAmbient(glm::vec3 col);
	std::shared_ptr<PointLight> getPointLight();
	std::shared_ptr<SpotLight> getSpotLight();
	std::shared_ptr<DirectionalLight> getDirectionalLight();
	std::shared_ptr<AmbientLight> getAmbient();

	~LightingComponent();
	static  ComponentEnum getType();
private:
	std::shared_ptr<PointLight> pointLight;
	std::shared_ptr<SpotLight> spotLight;
	std::shared_ptr<DirectionalLight> directionalLight;
	std::shared_ptr<AmbientLight> ambientLight;
};

#endif // LIGHTING_COMPONENT_H

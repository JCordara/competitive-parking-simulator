#ifndef VOLUME_TRIGGER_COMPONENT_H
#define VOLUME_TRIGGER_COMPONENT_H

#include "Components.h"
#include <set>

class VolumeTriggerComponent : public BaseComponent {
public:

    VolumeTriggerComponent(Entity& e);

	void attachEntity(std::shared_ptr<Entity> collisionEntity);
	void removeEntity(std::shared_ptr<Entity> collisionEntity);
	void flush();
	void createVolumeShape(PxActor& actor);
    static  ComponentEnum getType();
    ~VolumeTriggerComponent();

private:
	std::set<std::shared_ptr<Entity>> attached;
};

#endif // VOLUME_TRIGGER_COMPONENT_H

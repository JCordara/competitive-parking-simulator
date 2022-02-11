#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "Components.h"


class ModelComponent : public BaseComponent {
public:
    ModelComponent(Entity& parent);
    ~ModelComponent();
    static ComponentEnum getType();
};

#endif // MODEL_COMPONENT_H

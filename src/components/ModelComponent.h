#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "BaseComponent.h"

class ModelComponent : public BaseComponent {
public:
    static ComponentEnum getType();
    ~ModelComponent();
};

#endif // MODEL_COMPONENT_H

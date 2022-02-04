#ifndef RENDERER_COMPONENT_H
#define RENDERER_COMPONENT_H

#include "BaseComponent.h"

class RendererComponent : public BaseComponent {
public:
    static  ComponentEnum getType();
    ~RendererComponent();
};

#endif // RENDERER_COMPONENT_H

#ifndef RENDERER_COMPONENT_H
#define RENDERER_COMPONENT_H

#include "Components.h"


class RendererComponent : public BaseComponent {
public:
    RendererComponent(Entity& parent);
    ~RendererComponent();
    static  ComponentEnum getType();
};

#endif // RENDERER_COMPONENT_H

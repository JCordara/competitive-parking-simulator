#ifndef RENDERER_COMPONENT_H
#define RENDERER_COMPONENT_H

#include "Components.h"


class RendererComponent : public BaseComponent {
public:
	RendererComponent(Entity& parent);
	void enableRender() { render = true; }
	void disableRender() { render = false; }
	bool toBeRendered() { return render; }
    ~RendererComponent();
    static  ComponentEnum getType();
private:
	bool render = false;
};

#endif // RENDERER_COMPONENT_H

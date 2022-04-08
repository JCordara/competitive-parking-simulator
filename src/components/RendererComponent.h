#ifndef RENDERER_COMPONENT_H
#define RENDERER_COMPONENT_H

#include "Components.h"


class RendererComponent : public BaseComponent {
public:
	RendererComponent(weak_ptr<Entity> parent);
	void enableRender() { render = true; }
	void disableRender() { render = false; }
	bool toBeRendered() { return render; }
	void enableTransparentRendering() { transparent = true; }
	void disableTransparentRendering() { transparent = false; }
	bool isTransparent() { return transparent; }
    ~RendererComponent();
    static  ComponentEnum getType();
private:
	bool render = false;
	bool transparent = false;
};

#endif // RENDERER_COMPONENT_H

#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "Components.h"
#include <Model.h>


class ModelComponent : public BaseComponent {
public:
	ModelComponent(shared_ptr<Entity> parent);
	void setModel(std::shared_ptr<Model> model);
	std::shared_ptr<Model> getModel();
    ~ModelComponent();
    static ComponentEnum getType();
private:
	std::shared_ptr<Model> model;
};

#endif // MODEL_COMPONENT_H

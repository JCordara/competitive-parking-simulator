#include "ModelComponent.h"


ModelComponent::ModelComponent(weak_ptr<Entity> parent)
    : BaseComponent(parent)
{}

ComponentEnum ModelComponent::getType() {
    return ComponentEnum::model;
}

ModelComponent::~ModelComponent() {
    // Nothing to do here yet
}

void ModelComponent::setModel(std::shared_ptr<Model> model) { this->model = model; }
std::shared_ptr<Model> ModelComponent::getModel() { return model; }

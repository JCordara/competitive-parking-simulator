#include "ControlAxis.h"

/* Control Axes implementations */
ControlAxis::ControlAxis(
	int inputA, int inputB, Event<float>* event, TypeEnum type) 
	: value_(0.0f)
	, event_(event) 
	, inputA_(inputA)
	, inputB_(inputB)
	, valueA_(0.0f)
	, valueB_(0.0f)
	, type_(type)
{
	if (type_ == AXIS) {
		valueA_ = -1.0f;
		valueB_ = -1.0f;
	}
}

ControlAxis::ControlAxis(int inputA, int inputB, TypeEnum type)
	: ControlAxis(inputA, inputB, nullptr, type) {}

void ControlAxis::bindEvent(Event<float>* event) { event_ = event; }


void ControlAxis::setInputValue(int input, float value) {
	
	// Normlize axis values
	if (type_ == AXIS) value = (value + 1) * 0.5f;

	if      (input == inputA_) valueA_ = value;
	else if (input == inputB_) valueB_ = value;

	update();
}

void ControlAxis::update() {
	// Update the value
	float oldValue = value_;
	value_ = valueA_ - valueB_;
	
	if (type_ == SINGLE) value_ = valueA_;
	
	// If the value has changed, broadcast the event with new value
	// If an event is attached
	if (event_ != nullptr && value_ != oldValue)
		event_->broadcast(value_);
}

EntityControlAxis::EntityControlAxis(
	int inputA, int inputB, Event<Entity&, float>* event, Entity& e, TypeEnum type):
	ControlAxis(inputA, inputB, nullptr, type),
	event_(event),
	entity_(e)
{
	if (type_ == AXIS) {
		valueA_ = -1.0f;
		valueB_ = -1.0f;
	}
}

EntityControlAxis::EntityControlAxis(int inputA, int inputB, Entity& e, TypeEnum type)
	: EntityControlAxis(inputA, inputB, nullptr, e, type) {}

void EntityControlAxis::bindEvent(Event<Entity&, float>* event) { event_ = event; }

void EntityControlAxis::update() {
	// Update the value
	float oldValue = value_;
	value_ = valueA_ - valueB_;
	
	if (type_ == SINGLE) value_ = valueA_;
	
	// If the value has changed, broadcast the event with new value
	// If an event is attached
	if (event_ != nullptr && value_ != oldValue)
		event_->broadcast(entity_, value_);
}

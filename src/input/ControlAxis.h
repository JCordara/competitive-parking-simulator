#ifndef CONTROL_AXIS_H
#define CONTROL_AXIS_H

#include "Event.h"

/* Create an axis between two keys or axes */
class ControlAxis {
public:

	enum TypeEnum {
		KEY,
		AXIS,
		SINGLE
	};

	ControlAxis(int inputA, int inputB, TypeEnum type = KEY);
	ControlAxis(
		int inputA, int inputB, Event<float>* event, TypeEnum type = KEY);

	void bindEvent(Event<float>* event);
	float getValue() { return value_; }
	void setInputValue(int input, float value);
	void setType(TypeEnum type) { type_ = type; }
	virtual void update();


protected:
	// Type (0 = key, 1 = axis) determines how input values are treated
	// Axis inputs get normalized to [0.0, 1.0]
	TypeEnum type_;
	// Inputs
	int inputA_;
	int inputB_;
	// Input values
	float valueA_;
	float valueB_;
	// The resulting value of the axis
	float value_;

private:
	// The attached event that is triggered by this axis
	Event<float>* event_;
};

class EntityControlAxis : public ControlAxis {
public:
	EntityControlAxis(int inputA, int inputB, shared_ptr<Entity> e, TypeEnum type = KEY);
	EntityControlAxis(
		int inputA, int inputB, Event<shared_ptr<Entity>, float>* event, shared_ptr<Entity> e, TypeEnum type = KEY);
	void bindEvent(Event<shared_ptr<Entity>, float>* event);
	void update();
	
private:
	shared_ptr<Entity> entity_;
	Event<shared_ptr<Entity>, float>* event_;
};

#endif // INPUT_MANAGER_H

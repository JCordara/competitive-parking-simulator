#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Framework.h"
#include "Window.h"

#include <stdarg.h>


class GuiScene {
public:
	GuiScene(shared_ptr<Window> window);

	/* Render all GUI elements in the scene */
	void draw();

	void addLabel(
		float x, float y, 
		std::string text, 
		int emphasisLevel = 0
	);
	
	void addButton(
		float x, float y, 
		std::string text, 
		Event<void>& event, 
		int emphasisLevel = 0
	);
	
	void addCheckbox(
		float x, float y, 
		std::string text, 
		Event<bool>& event, 
		bool initialValue = false
	);
	
	void addSlider(
		float x, float y, 
		std::string text, 
		Event<float>& event, 
		float initialValue = 0.5f, 
		float min = 0.0f, 
		float max = 1.0f
	);

	void addSlider(
		float x, float y, 
		std::string text, 
		Event<int>& event, 
		int initialValue = 0.5f, 
		int min = 0.0f, 
		int max = 1.0f
	);

	bool gameWon;

private:

	typedef struct GUI_LABEL {

		GUI_LABEL(
			float _x, float _y, 
			std::string _text, int _emphasisLevel):
			x(_x), y(_y), 
			text(_text), emphasisLevel(_emphasisLevel) {}

		float x, y;
		std::string text;
		int emphasisLevel;
	} Label;

	typedef struct GUI_BUTTON {

		GUI_BUTTON(
			float _x, float _y, 
			std::string _text, 
			Event<void>& _event, int _emphasisLevel):
			x(_x), y(_y), 
			text(_text), 
			event(_event), emphasisLevel(_emphasisLevel) {}

		float x, y;
		std::string text;
		int emphasisLevel;
		Event<void>& event;
	} Button;

	typedef struct GUI_CHECKBOX {

		GUI_CHECKBOX(
			float _x, float _y, 
			std::string _text, 
			Event<bool>& _event, bool _initialValue): 
			x(_x), y(_y), 
			text(_text), 
			event(_event), v(_initialValue) {}

		float x, y;
		bool v;
		std::string text;
		Event<bool>& event;
	} Checkbox;

	typedef struct GUI_SLIDER_FLOAT {

		GUI_SLIDER_FLOAT(
			float _x, float _y, 
			std::string _text, 
			Event<float>& _event, 
			float _initialValue, float _min, float _max): 
			x(_x), y(_y), 
			text(_text), 
			event(_event), 
			v(_initialValue), min(_min), max(_max) {}

		float x, y;
		float v, min, max;
		std::string text;
		Event<float>& event;
	} SliderFloat;

	typedef struct GUI_SLIDER_INT {

		GUI_SLIDER_INT(
			float _x, float _y, 
			std::string _text, 
			Event<int>& _event, 
			int _initialValue, int _min, int _max): 
			x(_x), y(_y), 
			text(_text), 
			event(_event), 
			v(_initialValue), min(_min), max(_max) {}

		float x, y;
		int v, min, max;
		std::string text;
		Event<int>& event;
	} SliderInt;

	// Elements to render
	vector<Label>       labels;
	vector<Button>      buttons;
	vector<Checkbox>    checkboxes;
	vector<SliderFloat> floatSliders;
	vector<SliderInt>   intSliders;

	shared_ptr<Window> window;

	ImGuiWindowFlags windowFlags;

};

#endif

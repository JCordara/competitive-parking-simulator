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

	void addLabel(float x, float y, std::string text, int emphasisLevel = 0);
	void addButton(float x, float y, std::string text, Event<void>& event, int emphasisLevel = 0);
	void addCheckbox(float x, float y, std::string text, Event<bool>& event, bool initialValue = false);

	bool gameWon;

private:

	typedef struct GUI_LABEL {
		GUI_LABEL(float _x, float _y, std::string _text, int _emphasisLevel):
			x(_x), y(_y), text(_text), emphasisLevel(_emphasisLevel) {}
		float x, y;
		std::string text;
		int emphasisLevel;
	} Label;

	typedef struct GUI_BUTTON {
		GUI_BUTTON(float _x, float _y, std::string _text, Event<void>& _event, int _emphasisLevel):
			x(_x), y(_y), text(_text), event(_event), emphasisLevel(_emphasisLevel) {}
		float x, y;
		std::string text;
		int emphasisLevel;
		Event<void>& event;
	} Button;

	typedef struct GUI_CHECKBOX {
		GUI_CHECKBOX(float _x, float _y, std::string _text, Event<bool>& _event, bool _initialValue): 
			x(_x), y(_y), text(_text), event(_event), v(_initialValue) {}
		float x, y;
		bool v;
		std::string text;
		Event<bool>& event;
	} Checkbox;

	// Elements to render
	vector<Label>    labels;
	vector<Button>   buttons;
	vector<Checkbox> checkboxes;

	shared_ptr<Window> window;

	ImGuiWindowFlags windowFlags;

};

#endif

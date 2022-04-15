#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Framework.h"
#include "Window.h"
#include "Texture.h"

#include <stdarg.h>


class GuiScene {
public:
	GuiScene(shared_ptr<Window> window);
	~GuiScene();

	/* Render all GUI elements in the scene */
	void draw();

	void addLabel(
		float x, float y, 
		std::string text, 
		int emphasisLevel = 0
	);

	void addDynamicLabel(
		float x, float y,
		string* text,
		int emphasisLevel = 0
	);
	
	void addButton(
		int index,
		float x, float y, 
		std::string text, 
		Event<void>& event, 
		int emphasisLevel = 0
	);
	
	void addCheckbox(
		int index,
		float x, float y, 
		std::string text, 
		Event<bool>& event, 
		bool initialValue = false
	);

	void addCombo(
		int index,
		float x, float y,
		std::string name,
		std::vector<std::string> text,
		Event<int>& event,
		int initialValue = 0
	);
	
	void addSlider(
		int index,
		float x, float y, 
		std::string text, 
		Event<float>& event, 
		float initialValue = 0.5f, 
		float min = 0.0f, 
		float max = 1.0f
	);

	void addSlider(
		int index,
		float x, float y, 
		std::string text, 
		Event<int>& event, 
		int initialValue = 0.5f, 
		int min = 0.0f, 
		int max = 1.0f
	);

	void addImage(
		float x, float y, 
		float w, float h, 
		std::string filepath
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

	typedef struct GUI_DYNAMIC_LABEL {

		GUI_DYNAMIC_LABEL(
			float _x, float _y,
			string* _text, int _emphasisLevel) :
			x(_x), y(_y),
			text(_text), emphasisLevel(_emphasisLevel) {}

		float x, y;
		string* text;
		int emphasisLevel;
	} DynamicLabel;

	typedef struct GUI_BUTTON {

		GUI_BUTTON(
			int _ix,
			float _x, float _y, 
			std::string _text, 
			Event<void>& _event, int _emphasisLevel):
			index(_ix),
			x(_x), y(_y), 
			text(_text), 
			event(_event), emphasisLevel(_emphasisLevel) {}

		int index;
		float x, y;
		std::string text;
		int emphasisLevel;
		Event<void>& event;
	} Button;

	typedef struct GUI_CHECKBOX {

		GUI_CHECKBOX(
			int _ix,
			float _x, float _y, 
			std::string _text, 
			Event<bool>& _event, bool _initialValue): 
			index(_ix),
			x(_x), y(_y), 
			text(_text), 
			event(_event), v(_initialValue) {}

		int index;
		float x, y;
		bool v;
		std::string text;
		Event<bool>& event;
	} Checkbox;

	typedef struct GUI_COMBO {

		GUI_COMBO(
			int _ix,
			float _x, float _y,
			std::string _name,
			std::vector<std::string> _texts,
			Event<int>& _event, int _initialValue) :
			index(_ix),
			x(_x), y(_y),
			name(_name),
			texts(_texts),
			event(_event), v(_initialValue) {}

		int index;
		float x, y;
		int v;
		std::string name;
		std::vector<std::string> texts;
		Event<int>& event;
	} Combo;

	typedef struct GUI_SLIDER_FLOAT {

		GUI_SLIDER_FLOAT(
			int _ix,
			float _x, float _y, 
			std::string _text, 
			Event<float>& _event, 
			float _initialValue, float _min, float _max): 
			index(_ix),
			x(_x), y(_y), 
			text(_text), 
			event(_event), 
			v(_initialValue), min(_min), max(_max) {}

		int index;
		float x, y;
		float v, min, max;
		std::string text;
		Event<float>& event;
	} SliderFloat;

	typedef struct GUI_SLIDER_INT {

		GUI_SLIDER_INT(
			int _ix,
			float _x, float _y, 
			std::string _text, 
			Event<int>& _event, 
			int _initialValue, int _min, int _max): 
			index(_ix),
			x(_x), y(_y), 
			text(_text), 
			event(_event), 
			v(_initialValue), min(_min), max(_max) {}

		int index;
		float x, y;
		int v, min, max;
		std::string text;
		Event<int>& event;
	} SliderInt;

	typedef struct GUI_IMAGE {

		GUI_IMAGE(
			float _x, float _y, float _w, float _h, shared_ptr<Texture> _image): 
			x(_x), y(_y), w(_w), h(_h), image(_image) {}

		float x, y;
		float w, h;
		shared_ptr<Texture> image;
	} Image;

	// Elements to render
	vector<Label>        labels;
	vector<DynamicLabel> dynamicLabels;
	vector<Button>       buttons;
	vector<Checkbox>     checkboxes;
	vector<Combo>        combos;
	vector<SliderFloat>  floatSliders;
	vector<SliderInt>    intSliders;
	vector<Image>   	 images;

	Button*      buttonEvent = nullptr;
	Checkbox*    checkboxEvent = nullptr;
	Combo*       comboEvent = nullptr;
	SliderFloat* floatSliderEvent = nullptr;
	SliderInt*   intSliderEvent = nullptr;

	shared_ptr<Window> window;

	ImGuiWindowFlags windowFlags;

	ImVec4 highlightColor = ImVec4(0.7f, 0.7f, 0.2f, 1.0f);
	int selectedElement = -1;
	int maxIndex = -1;

	void menuNav (float v);
	void menuSelect ();

};

char** stringArrayToConstCharArray(std::string* arr, unsigned int count);


#endif

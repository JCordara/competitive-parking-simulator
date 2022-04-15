#include "GuiScene.h"
#include "Texture.h"

GuiScene::GuiScene(shared_ptr<Window> window):
	window(window)
{
	windowFlags = 
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove 	  |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_AlwaysAutoResize;
	
	ImGui::StyleColorsClassic();

	Events::StickMoveY.registerHandler<GuiScene, &GuiScene::menuNav>(this);
	Events::APressed.registerHandler<GuiScene, &GuiScene::menuSelect>(this);
}

void GuiScene::draw() {

	glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

	// Starting the new ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Cover entire screen with transparent GUI window
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(
		ImVec2(static_cast<float>(window->getWidth()), static_cast<float>(window->getHeight())), 
		ImGuiCond_FirstUseEver); 

	std::vector<char*> toDelete;

	// Render the scene's elements
	if (ImGui::Begin("GUI LAYER", nullptr, windowFlags)) {

		// Render images first (behind other things)
		for (auto& image : images) {
			float x = window->getWidth() * image.x;
			float y = window->getHeight() * image.y;
			float w = window->getWidth() * image.w;
			float h = window->getHeight() * image.h;
			ImGui::SetCursorScreenPos(ImVec2(x, y));
			ImGui::Image((void*)(intptr_t)image.image->getTextureHandleID(), ImVec2(w, h));
		}

		for (auto& label : labels) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * label.x, window->getHeight() * label.y));
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[label.emphasisLevel]);
			ImGui::Text(label.text.c_str());
			ImGui::PopFont();
		}

		for (auto& label : dynamicLabels) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth() * label.x, window->getHeight() * label.y));
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[label.emphasisLevel]);
			ImGui::Text((*(label.text)).c_str());
			ImGui::PopFont();
		}

		for (auto& button : buttons) {
			// Calculate button size
			float btnWidth = std::max(100.0f, (button.text.length() * 20.0f) ) * ((button.emphasisLevel + 1) * 0.5f);
			float btnHeight = std::max(30.0f, (button.emphasisLevel * 35.0f) + 15);
			// Render button
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * button.x, window->getHeight() * button.y));
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[button.emphasisLevel]);
			if (selectedElement == button.index) ImGui::PushStyleColor(ImGuiCol_Button, highlightColor);
			if (ImGui::Button(button.text.c_str(), ImVec2(btnWidth, btnHeight)))
				buttonEvent = &button;
			if (selectedElement == button.index) ImGui::PopStyleColor();
			ImGui::PopFont();
		}

		for (auto& checkbox : checkboxes) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * checkbox.x, window->getHeight() * checkbox.y));
			if (selectedElement == checkbox.index) ImGui::PushStyleColor(ImGuiCol_Button, highlightColor);
			if (ImGui::Checkbox(checkbox.text.c_str(), &checkbox.v))
				checkboxEvent = &checkbox;
			if (selectedElement == checkbox.index) ImGui::PopStyleColor();
		}

		for (auto& combo : combos) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth() * combo.x, window->getHeight() * combo.y));
			auto v = stringArrayToConstCharArray(combo.texts.data(), combo.texts.size());
			if (selectedElement == combo.index) ImGui::PushStyleColor(ImGuiCol_Button, highlightColor);
			if (ImGui::Combo(combo.name.c_str(), &combo.v ,v, combo.texts.size()))
				comboEvent = &combo;
			for(int i = 0 ; i < combo.texts.size(); i++)
				toDelete.push_back(v[i]);
			if (selectedElement == combo.index) ImGui::PopStyleColor();
		}

		for (auto& slider : floatSliders) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * slider.x, window->getHeight() * slider.y));
			ImGui::PushItemWidth(window->getWidth() * 0.15f);
			if (selectedElement == slider.index) {
				if (uiState == eNONE_STATE) ImGui::PushStyleColor(ImGuiCol_FrameBg, highlightColor);
				if (uiState == eSLIDER_STATE) ImGui::PushStyleColor(ImGuiCol_SliderGrab, highlightColor);
			}
			if (ImGui::SliderFloat(slider.text.c_str(), &slider.v, slider.min, slider.max, "%.2f"))
				floatSliderEvent = &slider;
			ImGui::PopItemWidth();
			if (selectedElement == slider.index) ImGui::PopStyleColor();
		}

		for (auto& slider : intSliders) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * slider.x, window->getHeight() * slider.y));
			ImGui::PushItemWidth(window->getWidth() * 0.15f);
			if (selectedElement == slider.index) {
				if (uiState == eNONE_STATE) ImGui::PushStyleColor(ImGuiCol_FrameBg, highlightColor);
				if (uiState == eSLIDER_STATE) ImGui::PushStyleColor(ImGuiCol_SliderGrab, highlightColor);
			}
			if (ImGui::SliderInt(slider.text.c_str(), &slider.v, slider.min, slider.max))
				intSliderEvent = &slider;
			ImGui::PopItemWidth();
			if (selectedElement == slider.index) ImGui::PopStyleColor();
		}

		ImGui::SetCursorScreenPos(ImVec2(window->getWidth() * 0.005f, window->getHeight() * 0.005f));
		ImGui::Text("FPS: %.0f", Time::fps());

		ImGui::End();
	}

	// Broadcast any events after rendering finished
	if (!buttons.empty()) if (buttonEvent) buttonEvent->event.broadcast();
	if (!checkboxes.empty()) if (checkboxEvent) checkboxEvent->event.broadcast(checkboxEvent->v);
	if (!combos.empty()) if (comboEvent) comboEvent->event.broadcast(comboEvent->v);
	if (!floatSliders.empty()) if (floatSliderEvent) floatSliderEvent->event.broadcast(floatSliderEvent->v);
	if (!intSliders.empty()) if (intSliderEvent) intSliderEvent->event.broadcast(intSliderEvent->v);

	buttonEvent = nullptr;
	checkboxEvent = nullptr;
	comboEvent = nullptr;
	floatSliderEvent = nullptr;
	intSliderEvent = nullptr;

	// Render the ImGui window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

	for (auto p : toDelete) delete[] p;

	glEnable(GL_FRAMEBUFFER_SRGB);

}

void GuiScene::addLabel(float x, float y, std::string text, int emphasisLevel) {
	labels.emplace_back(x, y, text, emphasisLevel);
}

void GuiScene::addDynamicLabel(float x, float y, string* text, int emphasisLevel) {
	dynamicLabels.emplace_back(x, y, text, emphasisLevel);
}

void GuiScene::addButton(int index, float x, float y, std::string text, Event<void>& event, int emphasisLevel) {
	buttons.emplace_back(index, x, y, text, event, emphasisLevel);
	if (index > maxIndex) maxIndex = index;
}

void GuiScene::addCheckbox(int index, float x, float y, std::string text, Event<bool>& event, bool initialValue) {
	checkboxes.emplace_back(index, x, y, text, event, initialValue);
	if (index > maxIndex) maxIndex = index;
}

void GuiScene::addCombo(int index, float x, float y, std::string name, std::vector<std::string> text, Event<int>& event, int initialValue) {
	combos.emplace_back(index, x, y, name, text, event, initialValue);
	if (index > maxIndex) maxIndex = index;
}

void GuiScene::addSlider(int index, float x, float y, std::string text, Event<float>& event, float initialValue, float min, float max) {
	floatSliders.emplace_back(index, x, y, text, event, initialValue, min, max);
	if (index > maxIndex) maxIndex = index;
}

void GuiScene::addSlider(int index, float x, float y, std::string text, Event<int>& event, int initialValue, int min, int max) {
	intSliders.emplace_back(index, x, y, text, event, initialValue, min, max);
	if (index > maxIndex) maxIndex = index;
}

void GuiScene::addImage(float x, float y, float w, float h, std::string filepath) {
	auto t = make_shared<Texture>();
	images.emplace_back(x, y, w, h, t);
	images.back().image->load(filepath.c_str(), GL_LINEAR, false);
}


void GuiScene::menuNav (float v) {
	
	switch (uiState) {
		case eNONE_STATE:
			if (selectedElement < 0) selectedElement = maxIndex;
			if (selectedElement > maxIndex) selectedElement = 0;
			if (v == 1.0f) selectedElement++;
			else if (v == -1.0f) selectedElement--;
			break;

		case eCOMBO_STATE:
			break;

		case eSLIDER_STATE:
			for (auto& slider : floatSliders) {
				if (slider.index == selectedElement) {
					float sliderStep = (slider.max - slider.min) / 20.0f;
					if (v == 1.0f) {
						slider.v -= sliderStep;
						slider.event.broadcast(slider.v);
					}
					else if (v == -1.0f) {
						slider.v += sliderStep;
						slider.event.broadcast(slider.v);
					}
					if (slider.v <= slider.min) slider.v = slider.min;
					if (slider.v >= slider.max) slider.v = slider.max;
					break;
				}
			}
			for (auto& slider : intSliders) {
				if (slider.index == selectedElement) {
					if (v == 1.0f) {
						slider.v -= 1;	
						slider.event.broadcast(slider.v);
					}
					else if (v == -1.0f) {
						slider.v += 1;
						slider.event.broadcast(slider.v);
					}
					if (slider.v <= slider.min) slider.v = slider.min;
					if (slider.v >= slider.max) slider.v = slider.max;
					break;
				}
			}
			break;
	};
}

void GuiScene::menuSelect () {

	// Get the type of element that is currently highlighted by the controller
	ElementType selectedType = getSelectedType();

	// Define behavior depending on type of element selected
	switch (selectedType) {
		case eBUTTON:
			for (auto& button : buttons) 
				if (button.index == selectedElement) buttonEvent = &button;
			break;

		case eCHECKBOX:
			for (auto& checkbox : checkboxes) 
				if (checkbox.index == selectedElement) {
					checkboxEvent = &checkbox;
					checkbox.v = !checkbox.v;
				}
			break;

		case eCOMBO:
			/* for (auto& combo : combos) {
				if (combo.index == selectedElement) {
					// Behavior when 'A' is pressed on a dropdown menu
					if (uiState == eNONE_STATE) {
						uiState = eCOMBO_STATE;
					}
					else if (uiState == eCOMBO_STATE) {
						uiState = eNONE_STATE;
						comboEvent = &combo;
					}
				}
			} */
			break;

		case eSLIDER_FLOAT:
			for (auto& slider : floatSliders) {
				if (slider.index == selectedElement) {
					// Behavior when 'A' is pressed on a slider
					if 		(uiState == eNONE_STATE) uiState = eSLIDER_STATE;
					else if (uiState == eSLIDER_STATE) uiState = eNONE_STATE;
				}
			}
			break;

		case eSLIDER_INT:
			for (auto& slider : intSliders) {
				if (slider.index == selectedElement) {
					// Behavior when 'A' is pressed on a slider
					if 		(uiState == eNONE_STATE) uiState = eSLIDER_STATE;
					else if (uiState == eSLIDER_STATE) uiState = eNONE_STATE;
				}
			}
			break; 
	};
}

GuiScene::ElementType GuiScene::getSelectedType() {
	for (auto& button : buttons) 
		if (button.index == selectedElement) return GuiScene::eBUTTON;
	for (auto& checkbox : checkboxes) 
		if (checkbox.index == selectedElement) return GuiScene::eCHECKBOX;
	for (auto& combo : combos) 
		if (combo.index == selectedElement) return GuiScene::eCOMBO;
	for (auto& slider : floatSliders) 
		if (slider.index == selectedElement) return GuiScene::eSLIDER_FLOAT;
	for (auto& slider : intSliders) 
		if (slider.index == selectedElement) return GuiScene::eSLIDER_INT;
}



GuiScene::~GuiScene() {
	labels.clear();
	dynamicLabels.clear();
	buttons.clear();
	checkboxes.clear();
	combos.clear();
	floatSliders.clear();
	intSliders.clear();
	images.clear();

	Events::StickMoveY.unregisterHandler<GuiScene, &GuiScene::menuNav>(this);
	Events::APressed.unregisterHandler<GuiScene, &GuiScene::menuSelect>(this);
}



char** stringArrayToConstCharArray(std::string* arr, unsigned int count) {
	char** ret = new char*[count];
	for (int i = 0; i < count; i++) {
		ret[i] = new char[arr[i].size() + 1];
		for (int j = 0; j < arr[i].size(); j++)
			ret[i][j] = arr[i][j];
		ret[i][arr[i].size()] = '\0';
	}
	return ret;
}

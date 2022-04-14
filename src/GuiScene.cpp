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

	vector<Button> buttonEvents;
	vector<Checkbox> checkboxEvents;
	vector<Combo> comboEvents;
	vector<SliderFloat> floatSliderEvents;
	vector<SliderInt> intSliderEvents;

	std::vector<char*> toDelete;

	// Render the scene's elements
	if (ImGui::Begin("GUI LAYER", nullptr, windowFlags)) {

		ImGui::Text("FPS: %.2f", Time::fps());

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
			if (ImGui::Button(button.text.c_str(), ImVec2(btnWidth, btnHeight)))
				buttonEvents.push_back(button);
			ImGui::PopFont();
		}

		for (auto& checkbox : checkboxes) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * checkbox.x, window->getHeight() * checkbox.y));
			if (ImGui::Checkbox(checkbox.text.c_str(), &checkbox.v))
				checkboxEvents.push_back(checkbox);
		}

		for (auto& combo : combos) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth() * combo.x, window->getHeight() * combo.y));
			auto v = stringArrayToConstCharArray(combo.texts.data(), combo.texts.size());
			if (ImGui::Combo(combo.name.c_str(), &combo.v ,v, combo.texts.size()))
				comboEvents.push_back(combo);
			for(int i = 0 ; i < combo.texts.size(); i++)
				toDelete.push_back(v[i]);
		}

		for (auto& slider : floatSliders) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * slider.x, window->getHeight() * slider.y));
			ImGui::PushItemWidth(window->getWidth() * 0.15f);
			if (ImGui::SliderFloat(slider.text.c_str(), &slider.v, slider.min, slider.max, "%.2f"))
				floatSliderEvents.push_back(slider);
			ImGui::PopItemWidth();
		}

		for (auto& slider : intSliders) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * slider.x, window->getHeight() * slider.y));
			ImGui::PushItemWidth(window->getWidth() * 0.15f);
			if (ImGui::SliderInt(slider.text.c_str(), &slider.v, slider.min, slider.max))
				intSliderEvents.push_back(slider);
			ImGui::PopItemWidth();
		}


		ImGui::End();
	}

	// Broadcast any events after rendering finished
	if (buttonEvents.size() > 0) buttonEvents[0].event.broadcast();
	if (checkboxEvents.size() > 0) checkboxEvents[0].event.broadcast(checkboxEvents[0].v);
	if (comboEvents.size() > 0) comboEvents[0].event.broadcast(comboEvents[0].v);
	if (floatSliderEvents.size() > 0) floatSliderEvents[0].event.broadcast(floatSliderEvents[0].v);
	if (intSliderEvents.size() > 0) intSliderEvents[0].event.broadcast(intSliderEvents[0].v);

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

void GuiScene::addButton(float x, float y, std::string text, Event<void>& event, int emphasisLevel) {
	buttons.emplace_back(x, y, text, event, emphasisLevel);
}

void GuiScene::addCheckbox(float x, float y, std::string text, Event<bool>& event, bool initialValue) {
	checkboxes.emplace_back(x, y, text, event, initialValue);
}

void GuiScene::addCombo(float x, float y, std::string name, std::vector<std::string> text, Event<int>& event, int initialValue) {
	combos.emplace_back(x, y, name, text, event, initialValue);
}

void GuiScene::addSlider(float x, float y, std::string text, Event<float>& event, float initialValue, float min, float max) {
	floatSliders.emplace_back(x, y, text, event, initialValue, min, max);
}

void GuiScene::addSlider(float x, float y, std::string text, Event<int>& event, int initialValue, int min, int max) {
	intSliders.emplace_back(x, y, text, event, initialValue, min, max);
}

void GuiScene::addImage(float x, float y, float w, float h, std::string filepath) {
	auto t = make_shared<Texture>();
	images.emplace_back(x, y, w, h, t);
	images.back().image->load(filepath.c_str(), GL_LINEAR, false);
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

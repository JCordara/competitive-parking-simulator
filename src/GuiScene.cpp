#include "GuiScene.h"


extern std::unordered_map<unsigned int, int> scores;
extern unsigned int playerId;
extern std::vector<unsigned int> aiList;
extern std::shared_ptr<Entity> playerCar;

bool gameWon = false;

GuiScene::GuiScene(shared_ptr<Window> window):
	window(window)
{
	windowFlags = 
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove 	  |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_AlwaysAutoResize;
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

	if (ImGui::Begin("GUI LAYER", nullptr, windowFlags)) {

		// Render the scene's elements
		for (auto& label : labels) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * label.x, window->getHeight() * label.y));
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[label.emphasisLevel]);
			ImGui::Text(label.text.c_str());
			ImGui::PopFont();
		}

		for (auto& button : buttons) {
			// Calculate button size
			float btnWidth = std::max(100.0f, (button.text.length() * 20.0f) ) * ((button.emphasisLevel + 1) * 0.5);
			float btnHeight = std::max(30.0f, (button.emphasisLevel * 35.0f) + 15);
			// Render button
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * button.x, window->getHeight() * button.y));
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[button.emphasisLevel]);
			if (ImGui::Button(button.text.c_str(), ImVec2(btnWidth, btnHeight)))
				button.event.broadcast();
			ImGui::PopFont();
		}

		for (auto& checkbox : checkboxes) {
			ImGui::SetCursorScreenPos(ImVec2(window->getWidth()  * checkbox.x, window->getHeight() * checkbox.y));
			if (ImGui::Checkbox(checkbox.text.c_str(), &checkbox.v))
				checkbox.event.broadcast(checkbox.v);
		}

		ImGui::End();
	}

	// Render the ImGui window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void GuiScene::addLabel(float x, float y, std::string text, int emphasisLevel) {
	labels.emplace_back(x, y, text, emphasisLevel);
}

void GuiScene::addButton(float x, float y, std::string text, Event<void>& event, int emphasisLevel) {
	buttons.emplace_back(x, y, text, event, emphasisLevel);
}

void GuiScene::addCheckbox(float x, float y, std::string text, Event<bool>& event, bool initialValue) {
	checkboxes.emplace_back(x, y, text, event, initialValue);
}



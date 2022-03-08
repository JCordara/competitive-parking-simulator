
#include "GUI.h"

extern std::unordered_map<unsigned int, int> scores;
extern unsigned int playerId;
extern std::vector<unsigned int> aiList;

void GUI::draw() {

	glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

	// Starting the new ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(5, 5));

	// Setting flags
	ImGuiWindowFlags textWindowFlags =
        ImGuiWindowFlags_NoBackground |         // Disable drawing background color
		ImGuiWindowFlags_NoMove |				// text "window" should not move
		ImGuiWindowFlags_NoResize |				// should not resize
		ImGuiWindowFlags_NoCollapse |			// should not collapse
		ImGuiWindowFlags_NoSavedSettings |		// don't want saved settings mucking things up
		ImGuiWindowFlags_AlwaysAutoResize |		// window should auto-resize to fit the text
		ImGuiWindowFlags_NoTitleBar;			// no title; only the text should be visible


	ImGui::Begin("FPS counter", (bool*)0, textWindowFlags);

	ImGui::SetWindowFontScale(1.8f);
	ImGui::SetWindowSize(ImVec2(80, 25));

	ImGui::Text("FPS: %.2f", Time::fps());
	ImGui::Text("Player: %d", scores[playerId]);
	ImGui::Text("AI-1: %d", scores[aiList[0]]); // 1 AI HArdcoded, gross

	ImGui::End();

	// Render the ImGui window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

}

GUI::~GUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

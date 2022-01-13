#pragma once


/*
class testCallback : public CallbackInterface {
public:
	struct testParameters {
		glm::vec2 mousePos = glm::vec2(0.0f, 1.0f);
		bool ClickW_Pos_Edge_Trigger = false;
		bool ClickW_Neg_Edge_Trigger = false;
		bool ClickW_held = false;

		bool ClickS_Pos_Edge_Trigger = false;
		bool ClickS_Neg_Edge_Trigger = false;
		bool ClickS_held = false;

		bool ClickA_Pos_Edge_Trigger = false;
		bool ClickA_Neg_Edge_Trigger = false;
		bool ClickA_held = false;

		bool ClickD_Pos_Edge_Trigger = false;
		bool ClickD_Neg_Edge_Trigger = false;
		bool ClickD_held = false;
	};
	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			parameters.ClickW_held = true;
			parameters.ClickW_Pos_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			parameters.ClickW_held = false;
			parameters.ClickW_Neg_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			parameters.ClickS_held = true;
			parameters.ClickS_Pos_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			parameters.ClickS_held = false;
			parameters.ClickS_Neg_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			parameters.ClickA_held = true;
			parameters.ClickA_Pos_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			parameters.ClickA_held = false;
			parameters.ClickA_Neg_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			parameters.ClickD_held = true;
			parameters.ClickD_Pos_Edge_Trigger = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			parameters.ClickD_held = false;
			parameters.ClickD_Neg_Edge_Trigger = true;
		}
	}
	virtual void mouseButtonCallback(int button, int action, int mods) {
		//Do Nothing
	}
	virtual void cursorPosCallback(double xpos, double ypos) {
		if (xpos < 0) xScreenPos = 0;
		else if (xpos > windowWidth - 1) xScreenPos = windowWidth - 1;
		else xScreenPos = xpos;
		if (ypos < 0) yScreenPos = 0;
		else if (ypos > windowWidth - 1) yScreenPos = windowHeight - 1;
		else yScreenPos = ypos;
		parameters.mousePos = mouseGL();
	}
	virtual void scrollCallback(double xoffset, double yoffset) {
		camera.incrementR(yoffset);
	}
	virtual void windowSizeCallback(int width, int height) {
		// The CallbackInterface::windowSizeCallback will call glViewport for us
		CallbackInterface::windowSizeCallback(width, height);
		aspect = float(width) / float(height);
	}
private:

}*/

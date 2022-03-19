#include "Window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// ----------------------
// non-static definitions
// ----------------------

Window::Window(
	int width, 
	int height,
	const char* title, 
	GLFWmonitor* monitor, 
	GLFWwindow* share
)
	: window(nullptr)
{
	// specify OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // needed for mac?
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// create window
	window = std::unique_ptr<GLFWwindow, WindowDeleter>(glfwCreateWindow(width, height, title, monitor, share));
	if (window == nullptr) {
		Log::error("WINDOW failed to create GLFW window");
		throw std::runtime_error("Failed to create GLFW window.");
	}
	glfwMakeContextCurrent(window.get());

	// initialize OpenGL extensions for the current context (this window)
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		Log::error("WINDOW glewInit error:");
		Log::error(std::string((char*)glewGetErrorString(err)));
		throw std::runtime_error("Failed to initialize GLEW");
	}

	glfwSetWindowSizeCallback(window.get(), defaultWindowSizeCallback);
	// Standard ImGui/GLFW middleware
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

Window::operator GLFWwindow*() {
	return window.get();
}

glm::ivec2 Window::getPos() const {
	int x, y;
	glfwGetWindowPos(window.get(), &x, &y);
	return glm::ivec2(x, y);
}


glm::ivec2 Window::getSize() const {
	int w, h;
	glfwGetWindowSize(window.get(), &w, &h);
	return glm::ivec2(w, h);
}

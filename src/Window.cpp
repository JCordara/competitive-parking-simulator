#include "Window.h"



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

	// Broadcast window size for anything that needs it
	Events::WindowResized.broadcast(width, height);

	// Standard ImGui/GLFW middleware
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	io.Fonts->AddFontFromFileTTF("fonts/segoeui.ttf", 20);
	io.Fonts->AddFontFromFileTTF("fonts/segoeui.ttf", 40);
	io.Fonts->AddFontFromFileTTF("fonts/segoeui.ttf", 60);
	io.Fonts->AddFontFromFileTTF("fonts/segoeuib.ttf", 100);
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

bool Window::isFullScreen() {
	return glfwGetWindowMonitor(window.get()) != NULL;
}

void Window::setFullScreen(int monitor){
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	if (monitor > 0 && monitor <= count) {
		//GLFWmonitor* mon = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitors[monitor - 1]);
		glfwSetWindowMonitor(
			window.get(),
			monitors[monitor - 1],
			0,
			0,
			mode->width,
			mode->height,
			mode->refreshRate
		);
	}
	else {
		glm::ivec2 pos = getPos(), size = getSize();
		glfwSetWindowMonitor(
			window.get(),
			NULL,
			pos.x + int(size.x * 0.05f),
			pos.y + int(size.y * 0.05f),
			int(size.x * 0.9f),
			int(size.y * 0.9f),
			GLFW_DONT_CARE
		);
	}
}
int Window::numberOfMonitors() {
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	return count;
}

int Window::getCurrentMonitorNumber() {
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	GLFWmonitor* currentMonitor = glfwGetWindowMonitor(window.get());
	for (int i = 0; i < count; i++)
		if (currentMonitor == monitors[i])
			return i + 1;
	return 0;
}

#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Framework.h"

extern int g_carsParked;

class GUI {
public:
	void draw();
    ~GUI();
};

#endif
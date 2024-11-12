#pragma once
#include "ext/imgui/imgui.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

namespace show {
namespace ui {
GLFWwindow *glfwWindowInit();
ImGuiIO &imguiInit(GLFWwindow* window, float font_size);
void loopStart(GLFWwindow *window, ImGuiIO &io);
void loopEnd(GLFWwindow *window, ImGuiIO &io);
void cleanup(GLFWwindow *window);
}
}


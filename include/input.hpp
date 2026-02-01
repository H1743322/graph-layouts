#pragma once
#include "camera.hpp"
#include <GLFW/glfw3.h>

void handleCameraInput(Camera2D &cam, GLFWwindow *window, float deltaTime);
void handleMouseCamera(Camera2D &cam, GLFWwindow *window);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

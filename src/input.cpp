#include "input.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <cmath>

static bool rightDragging = false;
static double lastX = 0.0, lastY = 0.0;

void handleCameraInput(Camera2D& cam, GLFWwindow* window, float dt) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    const float speed = 500.0f * dt / cam.zoom;
    const float zoomSpeed = 4.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.moveBy(0, speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.moveBy(0, -speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.moveBy(-speed, 0);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.moveBy(speed, 0);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.zoomBy(std::pow(zoomSpeed, dt));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam.zoomBy(std::pow(1.0f / zoomSpeed, dt));
}

void handleMouseCamera(Camera2D& cam, GLFWwindow* window) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!rightDragging) {
            rightDragging = true;
            lastX = x;
            lastY = y;
        } else {
            cam.moveBy(-(x - lastX) / cam.zoom, (y - lastY) / cam.zoom);
            lastX = x;
            lastY = y;
        }
    } else {
        rightDragging = false;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    Camera2D* cam = static_cast<Camera2D*>(glfwGetWindowUserPointer(window));
    if (!cam)
        return;

    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
        cam->zoomBy(yoffset > 0 ? 1.05f : 0.95f);
}

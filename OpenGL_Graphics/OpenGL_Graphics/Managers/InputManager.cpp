#include "stdafx.h"
#include "InputManager.h"

INPUTMANAGER& INPUTMANAGER::get_instance() {
    static INPUTMANAGER instance;
    return instance;
}

void INPUTMANAGER::initialize(GLFWwindow* window) {
    _window = window;
}

bool INPUTMANAGER::is_key_pressed(int key_code) const {
    if (!_window) return false;
    return glfwGetKey(_window, key_code) == GLFW_PRESS;
}

bool INPUTMANAGER::is_mouse_button_pressed(int button_code) const {
    if (!_window) return false;
    return glfwGetMouseButton(_window, button_code) == GLFW_PRESS;
}

void INPUTMANAGER::get_mouse_position(double& x, double& y) const {
    if (!_window) {
        x = 0.0;
        y = 0.0;
        return;
    }
    glfwGetCursorPos(_window, &x, &y);
}

void INPUTMANAGER::get_window_size(int& width, int& height) const {
    if (!_window) {
        width = 800;
        height = 600;
        return;
    }
    glfwGetWindowSize(_window, &width, &height);
}

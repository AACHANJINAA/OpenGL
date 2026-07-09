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

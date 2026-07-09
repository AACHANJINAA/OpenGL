#pragma once
#include <GLFW/glfw3.h>

class INPUTMANAGER {
private:
    GLFWwindow* _window = nullptr;

    INPUTMANAGER() = default;
    ~INPUTMANAGER() = default;
    INPUTMANAGER(const INPUTMANAGER&) = delete;
    INPUTMANAGER& operator=(const INPUTMANAGER&) = delete;

public:
    static INPUTMANAGER& get_instance();

    void initialize(GLFWwindow* window);
    bool is_key_pressed(int key_code) const;
};

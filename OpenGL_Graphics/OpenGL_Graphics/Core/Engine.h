#pragma once
#include <string>

struct GLFWwindow;

class ENGINE {
private:
    GLFWwindow* _window = nullptr;
    int _width;
    int _height;
    std::string _title;
    float _delta_time = 0.0f;
    float _last_frame = 0.0f;

    bool initialize_glfw();
    bool initialize_opengl();

public:
    ENGINE(int width = 800, int height = 600, const std::string& title = "OpenGL Component Engine");
    ~ENGINE();

    bool initialize();
    void run();
    void process_input();

    float get_delta_time() const { return _delta_time; }
    GLFWwindow* get_window() const { return _window; }
};

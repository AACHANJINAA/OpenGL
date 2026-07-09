#include "Engine.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../Managers/SceneManager.h"
#include "../Managers/ResourceManager.h"
#include "Shader.h"

// GLFW window size callback
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

ENGINE::ENGINE(int width, int height, const std::string& title)
    : _width(width), _height(height), _title(title) {}

ENGINE::~ENGINE() {
    RESOURCEMANAGER::get_instance().clear();
    SCENEMANAGER::get_instance().clear();
    if (_window) {
        glfwDestroyWindow(_window);
    }
    glfwTerminate();
}

bool ENGINE::initialize_glfw() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    _window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
    if (!_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, FramebufferSizeCallback);

    // Enable V-Sync
    glfwSwapInterval(1);

    return true;
}

bool ENGINE::initialize_opengl() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    return true;
}

bool ENGINE::initialize() {
    if (!initialize_glfw()) return false;
    if (!initialize_opengl()) return false;

    return true;
}

void ENGINE::process_input() {
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(_window, true);
    }
}

void ENGINE::run() {
    // Start SCENEMANAGER
    SCENEMANAGER::get_instance().start();

    _last_frame = static_cast<float>(glfwGetTime());

    double time_accumulator = 0.0;
    int frame_count = 0;

    while (!glfwWindowShouldClose(_window)) {
        // Calculate delta time
        float current_frame = static_cast<float>(glfwGetTime());
        _delta_time = current_frame - _last_frame;
        _last_frame = current_frame;

        // FPS calculation & window title update
        frame_count++;
        time_accumulator += _delta_time;
        if (time_accumulator >= 1.0) {
            double fps = static_cast<double>(frame_count) / time_accumulator;
            double ms = (time_accumulator / static_cast<double>(frame_count)) * 1000.0;
            
            char title_buf[128];
            snprintf(title_buf, sizeof(title_buf), "%s | FPS: %d (%.2f ms)", _title.c_str(), static_cast<int>(fps), ms);
            glfwSetWindowTitle(_window, title_buf);

            // Reset
            frame_count = 0;
            time_accumulator = 0.0;
        }

        // Process input
        process_input();

        // Clear color and depth buffers
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate projection and view matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        // Set view and projection uniforms
        auto shader = RESOURCEMANAGER::get_instance().get_shader("cubeShader");
        if (shader) {
            shader->use();
            shader->set_mat4("projection", projection);
            shader->set_mat4("view", view);
        }

        // Update, late update and render components
        SCENEMANAGER::get_instance().update(_delta_time);
        SCENEMANAGER::get_instance().late_update(_delta_time);
        SCENEMANAGER::get_instance().render();

        // Swap buffers and poll events
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    // End SCENEMANAGER
    SCENEMANAGER::get_instance().end();
}

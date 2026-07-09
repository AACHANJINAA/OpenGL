#include "Core/Engine.h"
#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Entities/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include <GLFW/glfw3.h>
#include <iostream>

// KEYBOARDCONTROLLER allows moving and rotating the cube manually.
class KEYBOARDCONTROLLER : public COMPONENT {
private:
    GLFWwindow* _window;
    float _move_speed;
    float _rotate_speed;

public:
    KEYBOARDCONTROLLER(GLFWwindow* window, float move_speed = 2.0f, float rotate_speed = 90.0f)
        : _window(window), _move_speed(move_speed), _rotate_speed(rotate_speed) {}

    void update(float delta_time) override {
        auto transform = _owner->get_component<TRANSFORMCOMPONENT>();
        if (!transform) return;

        // Check if 'R' key is pressed
        bool r_pressed = (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS);

        if (r_pressed) {
            // R + Arrow keys: Rotate the cube (Roll / Pitch)
            // Left/Right: Roll (Z-axis rotation)
            if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                transform->_rotation.z += _rotate_speed * delta_time;
            }
            if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                transform->_rotation.z -= _rotate_speed * delta_time;
            }
            // Up/Down: Pitch (X-axis rotation)
            if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
                transform->_rotation.x += _rotate_speed * delta_time;
            }
            if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                transform->_rotation.x -= _rotate_speed * delta_time;
            }
        } else {
            // Normal Arrow keys: Move the cube (Translate X / Y)
            if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                transform->_position.x -= _move_speed * delta_time;
            }
            if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                transform->_position.x += _move_speed * delta_time;
            }
            if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
                transform->_position.y += _move_speed * delta_time;
            }
            if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                transform->_position.y -= _move_speed * delta_time;
            }
        }

        // Reset angles to prevent overflow
        if (transform->_rotation.x > 360.0f) transform->_rotation.x -= 360.0f;
        if (transform->_rotation.x < -360.0f) transform->_rotation.x += 360.0f;
        if (transform->_rotation.y > 360.0f) transform->_rotation.y -= 360.0f;
        if (transform->_rotation.y < -360.0f) transform->_rotation.y += 360.0f;
        if (transform->_rotation.z > 360.0f) transform->_rotation.z -= 360.0f;
        if (transform->_rotation.z < -360.0f) transform->_rotation.z += 360.0f;
    }
};

int main() {
    ENGINE engine(800, 600, "OpenGL 3D Rainbow Cube (Manager-Component)");

    if (!engine.initialize()) {
        std::cerr << "Engine initialization failed!" << std::endl;
        return -1;
    }

    // 1. Load shader using RESOURCEMANAGER
    RESOURCEMANAGER::get_instance().load_shader("cubeShader", "Shaders/cube.vs", "Shaders/cube.fs");

    // 2. Create GAMEOBJECT using SCENEMANAGER
    auto cube_gameobject = SCENEMANAGER::get_instance().create_gameobject();

    // 3. Add components
    // 3-1. Add TRANSFORMCOMPONENT
    cube_gameobject->add_component<TRANSFORMCOMPONENT>(
        glm::vec3(0.0f, 0.0f, 0.0f),   // Position (center)
        glm::vec3(0.0f, 0.0f, 0.0f),   // Initial rotation (starts flat)
        glm::vec3(1.0f, 1.0f, 1.0f)    // Scale
    );

    // 3-2. Add MESHCOMPONENT
    cube_gameobject->add_component<MESHCOMPONENT>("cubeShader");

    // 3-3. Add KEYBOARDCONTROLLER (manual control with arrow keys and R key)
    cube_gameobject->add_component<KEYBOARDCONTROLLER>(engine.get_window(), 2.0f, 90.0f);

    // 4. Run engine main loop
    engine.run();

    return 0;
}

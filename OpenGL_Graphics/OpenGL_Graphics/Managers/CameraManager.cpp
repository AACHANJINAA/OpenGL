#include "stdafx.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include "imgui.h"

CAMERAMANAGER::CAMERAMANAGER()
    : _position(3.0f, 4.2426f, 3.0f) // Initial position placed to look down at origin at exactly 45 degrees
    , _target(0.0f, 0.0f, 0.0f)      // Looking at the origin (0, 0, 0)
    , _up(0.0f, 1.0f, 0.0f)
    , _fov(45.0f)
    , _near_plane(0.1f)
    , _far_plane(100.0f)
    , _front(-0.5f, -0.7071f, -0.5f) // Normalized vector pointing from (3, 4.24, 3) to (0, 0, 0)
    , _yaw(-135.0f)                  // Yaw angle for (-0.5, 0, -0.5) horizontal direction
    , _pitch(-45.0f)                 // Pitch angle looking down at exactly 45 degrees
    , _move_speed(3.0f)
    , _mouse_sensitivity(0.1f)
    , _last_mouse_x(400.0f)
    , _last_mouse_y(300.0f)
    , _first_mouse(true) {}

CAMERAMANAGER& CAMERAMANAGER::get_instance() {
    static CAMERAMANAGER instance;
    return instance;
}

glm::mat4 CAMERAMANAGER::get_view_matrix() const {
    // Return view matrix looking in the direction of the front vector
    return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4 CAMERAMANAGER::get_projection_matrix(float aspect_ratio) const {
    return glm::perspective(glm::radians(_fov), aspect_ratio, _near_plane, _far_plane);
}

void CAMERAMANAGER::set_position(const glm::vec3& pos) {
    _position = pos;
    _target = _position + _front;
}

glm::vec3 CAMERAMANAGER::get_position() const {
    return _position;
}

void CAMERAMANAGER::set_target(const glm::vec3& target_val) {
    _target = target_val;
    _front = glm::normalize(_target - _position);
    // Extract Yaw and Pitch from the new front vector
    _pitch = glm::degrees(std::asin(_front.y));
    _yaw = glm::degrees(std::atan2(_front.z, _front.x));
}

glm::vec3 CAMERAMANAGER::get_target() const {
    return _target;
}

void CAMERAMANAGER::set_fov(float fov_val) {
    _fov = fov_val;
}

float CAMERAMANAGER::get_fov() const {
    return _fov;
}

void CAMERAMANAGER::screen_point_to_ray(float x, float y, float width, float height, glm::vec3& ray_origin, glm::vec3& ray_dir) const {
    float x_ndc = (2.0f * x) / width - 1.0f;
    float y_ndc = 1.0f - (2.0f * y) / height;

    glm::vec4 ray_clip = glm::vec4(x_ndc, y_ndc, -1.0f, 1.0f);

    glm::mat4 projection = get_projection_matrix(width / height);
    glm::mat4 view = get_view_matrix();
    glm::mat4 inv_proj_view = glm::inverse(projection * view);

    glm::vec4 world_pos = inv_proj_view * ray_clip;
    
    if (std::abs(world_pos.w) > 0.0001f) {
        world_pos /= world_pos.w;
    }

    ray_origin = _position;
    ray_dir = glm::normalize(glm::vec3(world_pos) - ray_origin);
}

void CAMERAMANAGER::update(float delta_time) {
    auto& input = INPUTMANAGER::get_instance();

    // 1. Unreal-style Mouse Look (RMB Held) - Lock & hide cursor, restore on release
    static bool last_rmb_pressed = false;
    bool rmb_pressed = input.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT);
    
    GLFWwindow* window = input.get_window();
    if (window) {
        if (rmb_pressed && !last_rmb_pressed) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            _first_mouse = true;
        }
        else if (!rmb_pressed && last_rmb_pressed) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    last_rmb_pressed = rmb_pressed;
    
    if (rmb_pressed) {
        double current_x, current_y;
        input.get_mouse_position(current_x, current_y);

        if (_first_mouse) {
            _last_mouse_x = current_x;
            _last_mouse_y = current_y;
            _first_mouse = false;
        }

        float x_offset = static_cast<float>(current_x - _last_mouse_x);
        float y_offset = static_cast<float>(_last_mouse_y - current_y); // Y-axis inverted for window coords

        _last_mouse_x = current_x;
        _last_mouse_y = current_y;

        x_offset *= _mouse_sensitivity;
        y_offset *= _mouse_sensitivity;

        _yaw += x_offset;
        _pitch += y_offset;

        // Clamp Pitch bounds
        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;

        // Recalculate camera front vector
        glm::vec3 front;
        front.x = std::cos(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
        front.y = std::sin(glm::radians(_pitch));
        front.z = std::sin(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
        _front = glm::normalize(front);
        _target = _position + _front;
    } else {
        _first_mouse = true; // Reset mouse delta on release
    }

    // 2. Unreal-style Keyboard Movement (Only active when RMB is held)
    if (rmb_pressed) {
        float speed = _move_speed * delta_time;

        // Shift key boost speed
        if (input.is_key_pressed(GLFW_KEY_LEFT_SHIFT) || input.is_key_pressed(GLFW_KEY_RIGHT_SHIFT)) {
            speed *= 3.0f;
        }

        glm::vec3 right = glm::normalize(glm::cross(_front, glm::vec3(0.0f, 1.0f, 0.0f)));

        // W, S, A, D - horizontal movements
        if (input.is_key_pressed(GLFW_KEY_W)) {
            _position += _front * speed;
        }
        if (input.is_key_pressed(GLFW_KEY_S)) {
            _position -= _front * speed;
        }
        if (input.is_key_pressed(GLFW_KEY_A)) {
            _position -= right * speed;
        }
        if (input.is_key_pressed(GLFW_KEY_D)) {
            _position += right * speed;
        }

        // Q, E - vertical vertical flight
        if (input.is_key_pressed(GLFW_KEY_E)) {
            _position += glm::vec3(0.0f, 1.0f, 0.0f) * speed; // Up
        }
        if (input.is_key_pressed(GLFW_KEY_Q)) {
            _position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed; // Down
        }

        _target = _position + _front;
    }

    // 3. Focus on Selected Object ('F' key press)
    bool f_pressed = input.is_key_pressed(GLFW_KEY_F);
    static bool last_f_state = false;

    // Detect press edge (and make sure user isn't typing in an ImGui text input field)
    if (f_pressed && !last_f_state && !ImGui::GetIO().WantTextInput) {
        auto selected = SCENEMANAGER::get_instance().get_selected_gameobject();
        if (selected) {
            auto transform = selected->get_component<TRANSFORMCOMPONENT>();
            if (transform) {
                glm::vec3 target_pos = transform->_position;
                float focus_distance = 3.0f;

                // Move camera back along the front vector from the target position
                _position = target_pos - _front * focus_distance;
                _target = target_pos;
            }
        }
    }
    last_f_state = f_pressed;
}

#include "stdafx.h"
#include "KeyboardController.h"
#include "../Managers/InputManager.h"
#include "../Entities/GameObject.h"
#include "TransformComponent.h"
KEYBOARDCONTROLLER::KEYBOARDCONTROLLER(float move_speed, float rotate_speed)
    : _move_speed(move_speed), _rotate_speed(rotate_speed) {}

void KEYBOARDCONTROLLER::update(float delta_time) {
    auto transform = _owner->get_component<TRANSFORMCOMPONENT>();
    if (!transform) return;

    auto& input = INPUTMANAGER::get_instance();

    // Check if 'R' key is pressed
    bool r_pressed = input.is_key_pressed(GLFW_KEY_R);

    if (r_pressed) {
        // R + Arrow keys: Rotate the cube (Roll / Pitch)
        // Left/Right: Roll (Z-axis rotation)
        if (input.is_key_pressed(GLFW_KEY_LEFT)) {
            transform->_rotation.z += _rotate_speed * delta_time;
        }
        if (input.is_key_pressed(GLFW_KEY_RIGHT)) {
            transform->_rotation.z -= _rotate_speed * delta_time;
        }
        // Up/Down: Pitch (X-axis rotation)
        if (input.is_key_pressed(GLFW_KEY_UP)) {
            transform->_rotation.x += _rotate_speed * delta_time;
        }
        if (input.is_key_pressed(GLFW_KEY_DOWN)) {
            transform->_rotation.x -= _rotate_speed * delta_time;
        }
    } else {
        // Normal Arrow keys: Move the cube (Translate X / Y)
        if (input.is_key_pressed(GLFW_KEY_LEFT)) {
            transform->_position.x -= _move_speed * delta_time;
        }
        if (input.is_key_pressed(GLFW_KEY_RIGHT)) {
            transform->_position.x += _move_speed * delta_time;
        }
        if (input.is_key_pressed(GLFW_KEY_UP)) {
            transform->_position.y += _move_speed * delta_time;
        }
        if (input.is_key_pressed(GLFW_KEY_DOWN)) {
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

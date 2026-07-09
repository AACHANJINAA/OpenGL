#include "stdafx.h"
#include "CameraManager.h"

CAMERAMANAGER::CAMERAMANAGER()
    : _position(0.0f, 0.0f, 3.0f) // Camera is at (0, 0, 3)
    , _target(0.0f, 0.0f, 0.0f)   // Looking at (0, 0, 0)
    , _up(0.0f, 1.0f, 0.0f)
    , _fov(45.0f)
    , _near_plane(0.1f)
    , _far_plane(100.0f) {}

CAMERAMANAGER& CAMERAMANAGER::get_instance() {
    static CAMERAMANAGER instance;
    return instance;
}

glm::mat4 CAMERAMANAGER::get_view_matrix() const {
    return glm::lookAt(_position, _target, _up);
}

glm::mat4 CAMERAMANAGER::get_projection_matrix(float aspect_ratio) const {
    return glm::perspective(glm::radians(_fov), aspect_ratio, _near_plane, _far_plane);
}

void CAMERAMANAGER::set_position(const glm::vec3& pos) {
    _position = pos;
}

glm::vec3 CAMERAMANAGER::get_position() const {
    return _position;
}

void CAMERAMANAGER::set_target(const glm::vec3& target) {
    _target = target;
}

glm::vec3 CAMERAMANAGER::get_target() const {
    return _target;
}

void CAMERAMANAGER::set_fov(float fov) {
    _fov = fov;
}

float CAMERAMANAGER::get_fov() const {
    return _fov;
}

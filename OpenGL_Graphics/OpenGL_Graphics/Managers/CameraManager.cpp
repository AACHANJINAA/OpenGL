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

void CAMERAMANAGER::screen_point_to_ray(float x, float y, float width, float height, glm::vec3& ray_origin, glm::vec3& ray_dir) const {
    // 1. Convert screen coordinates to NDC (Normalized Device Coordinates)
    float x_ndc = (2.0f * x) / width - 1.0f;
    float y_ndc = 1.0f - (2.0f * y) / height;

    // 2. Homogeneous Clip Coordinates (Point on the near plane)
    glm::vec4 ray_clip = glm::vec4(x_ndc, y_ndc, -1.0f, 1.0f);

    // 3. Convert to World space using the inverse of Projection * View matrix
    glm::mat4 projection = get_projection_matrix(width / height);
    glm::mat4 view = get_view_matrix();
    glm::mat4 inv_proj_view = glm::inverse(projection * view);

    glm::vec4 world_pos = inv_proj_view * ray_clip;
    
    // 4. Perform perspective divide to get coordinates in 3D world space
    if (std::abs(world_pos.w) > 0.0001f) {
        world_pos /= world_pos.w;
    }

    // 5. Establish ray origin and normalized direction
    ray_origin = _position;
    ray_dir = glm::normalize(glm::vec3(world_pos) - ray_origin);
}

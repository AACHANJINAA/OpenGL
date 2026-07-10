#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CAMERAMANAGER {
private:
    glm::vec3 _position;
    glm::vec3 _target;
    glm::vec3 _up;
    float _fov;
    float _near_plane;
    float _far_plane;

    CAMERAMANAGER();
    ~CAMERAMANAGER() = default;
    CAMERAMANAGER(const CAMERAMANAGER&) = delete;
    CAMERAMANAGER& operator=(const CAMERAMANAGER&) = delete;

public:
    static CAMERAMANAGER& get_instance();

    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix(float aspect_ratio) const;

    void set_position(const glm::vec3& pos);
    glm::vec3 get_position() const;

    void set_target(const glm::vec3& target);
    glm::vec3 get_target() const;

    void set_fov(float fov);
    float get_fov() const;

    void screen_point_to_ray(float x, float y, float width, float height, glm::vec3& ray_origin, glm::vec3& ray_dir) const;
};

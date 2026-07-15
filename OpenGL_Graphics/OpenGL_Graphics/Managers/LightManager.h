#pragma once
#include <glm/glm.hpp>

class LIGHTMANAGER {
private:
    glm::vec3 _dir_light_direction;
    glm::vec3 _dir_light_color;
    float _dir_light_intensity;
    glm::vec3 _ambient_light;

    LIGHTMANAGER();
    ~LIGHTMANAGER() = default;
    LIGHTMANAGER(const LIGHTMANAGER&) = delete;
    LIGHTMANAGER& operator=(const LIGHTMANAGER&) = delete;

public:
    static LIGHTMANAGER& get_instance();

    // Getters and Setters for directional light properties
    glm::vec3 get_direction() const { return _dir_light_direction; }
    void set_direction(const glm::vec3& dir) { _dir_light_direction = glm::normalize(dir); }

    glm::vec3 get_color() const { return _dir_light_color; }
    void set_color(const glm::vec3& color) { _dir_light_color = color; }

    float get_intensity() const { return _dir_light_intensity; }
    void set_intensity(float intensity) { _dir_light_intensity = intensity; }

    glm::vec3 get_ambient() const { return _ambient_light; }
    void set_ambient(const glm::vec3& ambient) { _ambient_light = ambient; }
};

#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TRANSFORM_COMPONENT : public COMPONENT {
public:
    glm::vec3 _position;
    glm::vec3 _rotation; // Euler angles in degrees (Pitch, Yaw, Roll)
    glm::vec3 _scale;

    TRANSFORM_COMPONENT(const glm::vec3& pos = glm::vec3(0.0f),
                        const glm::vec3& rot = glm::vec3(0.0f),
                        const glm::vec3& scl = glm::vec3(1.0f));

    void initialize() override;
    void update(float delta_time) override;

    glm::mat4 get_model_matrix() const;
};

#include "stdafx.h"
#include "TransformComponent.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

TRANSFORMCOMPONENT::TRANSFORMCOMPONENT(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
    : _position(pos), _rotation(rot), _scale(scl) {}

void TRANSFORMCOMPONENT::start() {}

void TRANSFORMCOMPONENT::update(float delta_time) {}

glm::mat4 TRANSFORMCOMPONENT::get_model_matrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    
    float translation[3] = { _position.x, _position.y, _position.z };
    float rotation[3] = { _rotation.x, _rotation.y, _rotation.z };
    float scale[3] = { _scale.x, _scale.y, _scale.z };

    // Recompose the 4x4 matrix using the exact same mathematics and multiplication order as ImGuizmo.
    // This guarantees that the matrix composition and decomposition are fully symmetric, 
    // preventing any object jumps or axis flips.
    ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(model));
    
    return model;
}

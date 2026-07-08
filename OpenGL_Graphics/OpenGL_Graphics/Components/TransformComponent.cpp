#include "TransformComponent.h"

TRANSFORMCOMPONENT::TRANSFORMCOMPONENT(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
    : _position(pos), _rotation(rot), _scale(scl) {}

void TRANSFORMCOMPONENT::initialize() {}

void TRANSFORMCOMPONENT::update(float delta_time) {}

glm::mat4 TRANSFORMCOMPONENT::get_model_matrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, _position);
    model = glm::rotate(model, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, _scale);
    return model;
}

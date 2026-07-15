#include "stdafx.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "GeometryGenerator.h"
#include "../Entities/GameObject.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/CameraManager.h"
#include "../Managers/SceneManager.h"
#include "../Core/Shader.h"

MESHCOMPONENT::MESHCOMPONENT(const std::string& shader_name)
    : _shader_name(shader_name), _shape_type(SHAPETYPE::CUBE), _vao(0), _vbo(0), _ebo(0) {}

MESHCOMPONENT::MESHCOMPONENT(SHAPETYPE shape_type, const std::string& shader_name)
    : _shader_name(shader_name), _shape_type(shape_type), _vao(0), _vbo(0), _ebo(0) {}

MESHCOMPONENT::~MESHCOMPONENT() {
    if (_vao != 0) glDeleteVertexArrays(1, &_vao);
    if (_vbo != 0) glDeleteBuffers(1, &_vbo);
    if (_ebo != 0) glDeleteBuffers(1, &_ebo);
}

void MESHCOMPONENT::start() {
    // Generate shape vertices and indices based on SHAPETYPE
    switch (_shape_type) {
        case SHAPETYPE::CUBE:
            GEOMETRYGENERATOR::generate_cube(_vertices, _indices);
            break;
        case SHAPETYPE::SPHERE:
            GEOMETRYGENERATOR::generate_sphere(_vertices, _indices, 0.5f, 20, 20);
            break;
        case SHAPETYPE::PYRAMID:
            GEOMETRYGENERATOR::generate_pyramid(_vertices, _indices);
            break;
    }

    calculate_local_aabb();
    set_mesh();
}

void MESHCOMPONENT::calculate_local_aabb() {
    if (_vertices.empty()) {
        _aabb_min = glm::vec3(-0.5f);
        _aabb_max = glm::vec3(0.5f);
        return;
    }

    _aabb_min = _vertices[0]._position;
    _aabb_max = _vertices[0]._position;

    for (const auto& vertex : _vertices) {
        _aabb_min.x = std::min(_aabb_min.x, vertex._position.x);
        _aabb_min.y = std::min(_aabb_min.y, vertex._position.y);
        _aabb_min.z = std::min(_aabb_min.z, vertex._position.z);

        _aabb_max.x = std::max(_aabb_max.x, vertex._position.x);
        _aabb_max.y = std::max(_aabb_max.y, vertex._position.y);
        _aabb_max.z = std::max(_aabb_max.z, vertex._position.z);
    }
}

void MESHCOMPONENT::set_mesh() {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VERTEX), &_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)offsetof(VERTEX, _color));

    glBindVertexArray(0);
}

void MESHCOMPONENT::render() {
    auto shader = RESOURCEMANAGER::get_instance().get_shader(_shader_name);
    if (!shader) {
        std::cerr << "MESHCOMPONENT Error: Shader " << _shader_name << " not found in RESOURCEMANAGER." << std::endl;
        return;
    }

    auto transform = _owner->get_component<TRANSFORMCOMPONENT>();
    glm::mat4 model = (transform != nullptr) ? transform->get_model_matrix() : glm::mat4(1.0f);

    // Query dynamic viewport dimension to compute aspect ratio
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspect_ratio = (viewport[2] > 0 && viewport[3] > 0) ? (float)viewport[2] / (float)viewport[3] : 1.0f;

    // Retrieve projection and view matrices from CAMERAMANAGER
    glm::mat4 projection = CAMERAMANAGER::get_instance().get_projection_matrix(aspect_ratio);
    glm::mat4 view = CAMERAMANAGER::get_instance().get_view_matrix();

    shader->use();
    shader->set_mat4("projection", projection);
    shader->set_mat4("view", view);
    shader->set_mat4("model", model);

    // Set selection highlight uniform
    auto selected = SCENEMANAGER::get_instance().get_selected_gameobject();
    bool is_selected = (selected.get() == _owner);
    shader->set_float("u_selected", is_selected ? 1.0f : 0.0f);

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

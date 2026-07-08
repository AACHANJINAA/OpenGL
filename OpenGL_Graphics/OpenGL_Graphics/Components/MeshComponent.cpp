#include "MeshComponent.h"
#include "TransformComponent.h"
#include "../Entities/GameObject.h"
#include "../Managers/ResourceManager.h"
#include "../Core/Shader.h"
#include <GL/glew.h>
#include <iostream>

MESHCOMPONENT::MESHCOMPONENT(const std::string& shader_name)
    : _shader_name(shader_name), _vao(0), _vbo(0), _ebo(0) {}

MESHCOMPONENT::~MESHCOMPONENT() {
    if (_vao != 0) glDeleteVertexArrays(1, &_vao);
    if (_vbo != 0) glDeleteBuffers(1, &_vbo);
    if (_ebo != 0) glDeleteBuffers(1, &_ebo);
}

void MESHCOMPONENT::initialize() {
    // 3D Rainbow Cube vertices (position, color)
    _vertices = {
        // Front
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) }, // Red
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Green
        { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Blue
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 0.0f) }, // Yellow

        // Back
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 1.0f) }, // Magenta
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 1.0f) }, // Cyan
        { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f) }, // White
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.1f, 0.1f, 0.1f) }  // Dark Gray
    };

    _indices = {
        0, 1, 2, 2, 3, 0, // Front
        1, 5, 6, 6, 2, 1, // Right
        5, 4, 7, 7, 6, 5, // Back
        4, 0, 3, 3, 7, 4, // Left
        3, 2, 6, 6, 7, 3, // Top
        4, 5, 1, 1, 0, 4  // Bottom
    };

    set_mesh();
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

    shader->use();
    shader->set_mat4("model", model);

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

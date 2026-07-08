#pragma once
#include "Component.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct VERTEX {
    glm::vec3 _position;
    glm::vec3 _color;
};

class MESH_COMPONENT : public COMPONENT {
private:
    unsigned int _vao = 0;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
    std::vector<VERTEX> _vertices;
    std::vector<unsigned int> _indices;
    std::string _shader_name;

public:
    MESH_COMPONENT(const std::string& shader_name);
    ~MESH_COMPONENT() override;

    void setup_mesh();

    void initialize() override;
    void render() override;
};

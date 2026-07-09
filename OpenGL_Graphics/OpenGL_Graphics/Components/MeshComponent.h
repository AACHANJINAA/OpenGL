#pragma once
#include "Component.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct VERTEX {
    glm::vec3 _position;
    glm::vec3 _color;
};

class MESHCOMPONENT : public COMPONENT {
private:
    unsigned int _vao = 0;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
    std::vector<VERTEX> _vertices;
    std::vector<unsigned int> _indices;
    std::string _shader_name;

    void set_mesh();

public:
    MESHCOMPONENT(const std::string& shader_name);
    ~MESHCOMPONENT() override;

    void start() override;
    void render() override;
};

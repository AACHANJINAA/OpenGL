#pragma once
#include "Component.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

#ifndef VERTEX_DEFINED
#define VERTEX_DEFINED
struct VERTEX {
    glm::vec3 _position;
    glm::vec3 _color;
};
#endif

class MESHCOMPONENT : public COMPONENT {
public:
    enum class SHAPETYPE {
        CUBE,
        SPHERE,
        PYRAMID
    };

private:
    unsigned int _vao = 0;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
    std::vector<VERTEX> _vertices;
    std::vector<unsigned int> _indices;
    std::string _shader_name;
    SHAPETYPE _shape_type;

    // Bounding Box (AABB) in Local Space
    glm::vec3 _aabb_min = glm::vec3(-0.5f);
    glm::vec3 _aabb_max = glm::vec3(0.5f);

    void set_mesh();
    void calculate_local_aabb();

public:
    MESHCOMPONENT(const std::string& shader_name);
    MESHCOMPONENT(SHAPETYPE shape_type, const std::string& shader_name);
    ~MESHCOMPONENT() override;

    void start() override;
    void render() override;

    SHAPETYPE get_shape_type() const { return _shape_type; }
    glm::vec3 get_aabb_min() const { return _aabb_min; }
    glm::vec3 get_aabb_max() const { return _aabb_max; }
};

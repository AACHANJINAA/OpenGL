#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Define VERTEX with position, color, and normal for PBR/lighting.
#ifndef VERTEX_DEFINED
#define VERTEX_DEFINED
struct VERTEX {
    glm::vec3 _position;
    glm::vec3 _color;
    glm::vec3 _normal;
};
#endif

namespace GEOMETRYGENERATOR {
    inline void generate_cube(std::vector<VERTEX>& vertices, std::vector<unsigned int>& indices) {
        glm::vec3 default_color(0.8f, 0.8f, 0.8f);

        // A cube has 24 vertices to allow flat shading with distinct face normals
        vertices = {
            // Front face (normal = 0, 0, 1)
            { glm::vec3(-0.5f, -0.5f,  0.5f), default_color, glm::vec3(0.0f, 0.0f, 1.0f) },
            { glm::vec3( 0.5f, -0.5f,  0.5f), default_color, glm::vec3(0.0f, 0.0f, 1.0f) },
            { glm::vec3( 0.5f,  0.5f,  0.5f), default_color, glm::vec3(0.0f, 0.0f, 1.0f) },
            { glm::vec3(-0.5f,  0.5f,  0.5f), default_color, glm::vec3(0.0f, 0.0f, 1.0f) },

            // Back face (normal = 0, 0, -1)
            { glm::vec3(-0.5f, -0.5f, -0.5f), default_color, glm::vec3(0.0f, 0.0f, -1.0f) },
            { glm::vec3(-0.5f,  0.5f, -0.5f), default_color, glm::vec3(0.0f, 0.0f, -1.0f) },
            { glm::vec3( 0.5f,  0.5f, -0.5f), default_color, glm::vec3(0.0f, 0.0f, -1.0f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f), default_color, glm::vec3(0.0f, 0.0f, -1.0f) },

            // Top face (normal = 0, 1, 0)
            { glm::vec3(-0.5f,  0.5f, -0.5f), default_color, glm::vec3(0.0f, 1.0f, 0.0f) },
            { glm::vec3(-0.5f,  0.5f,  0.5f), default_color, glm::vec3(0.0f, 1.0f, 0.0f) },
            { glm::vec3( 0.5f,  0.5f,  0.5f), default_color, glm::vec3(0.0f, 1.0f, 0.0f) },
            { glm::vec3( 0.5f,  0.5f, -0.5f), default_color, glm::vec3(0.0f, 1.0f, 0.0f) },

            // Bottom face (normal = 0, -1, 0)
            { glm::vec3(-0.5f, -0.5f, -0.5f), default_color, glm::vec3(0.0f, -1.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f), default_color, glm::vec3(0.0f, -1.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f,  0.5f), default_color, glm::vec3(0.0f, -1.0f, 0.0f) },
            { glm::vec3(-0.5f, -0.5f,  0.5f), default_color, glm::vec3(0.0f, -1.0f, 0.0f) },

            // Left face (normal = -1, 0, 0)
            { glm::vec3(-0.5f, -0.5f, -0.5f), default_color, glm::vec3(-1.0f, 0.0f, 0.0f) },
            { glm::vec3(-0.5f, -0.5f,  0.5f), default_color, glm::vec3(-1.0f, 0.0f, 0.0f) },
            { glm::vec3(-0.5f,  0.5f,  0.5f), default_color, glm::vec3(-1.0f, 0.0f, 0.0f) },
            { glm::vec3(-0.5f,  0.5f, -0.5f), default_color, glm::vec3(-1.0f, 0.0f, 0.0f) },

            // Right face (normal = 1, 0, 0)
            { glm::vec3( 0.5f, -0.5f,  0.5f), default_color, glm::vec3(1.0f, 0.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f), default_color, glm::vec3(1.0f, 0.0f, 0.0f) },
            { glm::vec3( 0.5f,  0.5f, -0.5f), default_color, glm::vec3(1.0f, 0.0f, 0.0f) },
            { glm::vec3( 0.5f,  0.5f,  0.5f), default_color, glm::vec3(1.0f, 0.0f, 0.0f) }
        };

        indices = {
            0, 1, 2, 2, 3, 0,       // Front
            4, 5, 6, 6, 7, 4,       // Back
            8, 9, 10, 10, 11, 8,    // Top
            12, 13, 14, 14, 15, 12, // Bottom
            16, 17, 18, 18, 19, 16, // Left
            20, 21, 22, 22, 23, 20  // Right
        };
    }

    inline void generate_pyramid(std::vector<VERTEX>& vertices, std::vector<unsigned int>& indices) {
        glm::vec3 default_color(0.8f, 0.8f, 0.8f);

        glm::vec3 n_f = glm::normalize(glm::vec3(0.0f, 0.4472f, 0.8944f));
        glm::vec3 n_r = glm::normalize(glm::vec3(0.8944f, 0.4472f, 0.0f));
        glm::vec3 n_b = glm::normalize(glm::vec3(0.0f, 0.4472f, -0.8944f));
        glm::vec3 n_l = glm::normalize(glm::vec3(-0.8944f, 0.4472f, 0.0f));
        glm::vec3 n_down = glm::vec3(0.0f, -1.0f, 0.0f);

        vertices = {
            // Front face
            { glm::vec3( 0.0f,  0.5f,  0.0f), default_color, n_f }, // Apex
            { glm::vec3(-0.5f, -0.5f,  0.5f), default_color, n_f },
            { glm::vec3( 0.5f, -0.5f,  0.5f), default_color, n_f },

            // Right face
            { glm::vec3( 0.0f,  0.5f,  0.0f), default_color, n_r }, // Apex
            { glm::vec3( 0.5f, -0.5f,  0.5f), default_color, n_r },
            { glm::vec3( 0.5f, -0.5f, -0.5f), default_color, n_r },

            // Back face
            { glm::vec3( 0.0f,  0.5f,  0.0f), default_color, n_b }, // Apex
            { glm::vec3( 0.5f, -0.5f, -0.5f), default_color, n_b },
            { glm::vec3(-0.5f, -0.5f, -0.5f), default_color, n_b },

            // Left face
            { glm::vec3( 0.0f,  0.5f,  0.0f), default_color, n_l }, // Apex
            { glm::vec3(-0.5f, -0.5f, -0.5f), default_color, n_l },
            { glm::vec3(-0.5f, -0.5f,  0.5f), default_color, n_l },

            // Base face (flat normals down)
            { glm::vec3(-0.5f, -0.5f,  0.5f), default_color, n_down },
            { glm::vec3( 0.5f, -0.5f,  0.5f), default_color, n_down },
            { glm::vec3( 0.5f, -0.5f, -0.5f), default_color, n_down },
            { glm::vec3(-0.5f, -0.5f, -0.5f), default_color, n_down }
        };

        indices = {
            0, 1, 2,        // Front
            3, 4, 5,        // Right
            6, 7, 8,        // Back
            9, 10, 11,      // Left
            12, 13, 14,     // Base 1
            14, 15, 12      // Base 2
        };
    }

    inline void generate_sphere(std::vector<VERTEX>& vertices, std::vector<unsigned int>& indices, float radius = 0.5f, unsigned int segments = 16, unsigned int rings = 16) {
        vertices.clear();
        indices.clear();

        glm::vec3 default_color(0.8f, 0.8f, 0.8f);

        for (unsigned int y = 0; y <= rings; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)rings;
                float xPos = std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
                float yPos = std::cos(ySegment * glm::pi<float>());
                float zPos = std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

                glm::vec3 pos = glm::vec3(xPos, yPos, zPos) * radius;
                glm::vec3 normal = glm::normalize(glm::vec3(xPos, yPos, zPos));

                vertices.push_back({ pos, default_color, normal });
            }
        }

        for (unsigned int y = 0; y < rings; ++y) {
            for (unsigned int x = 0; x < segments; ++x) {
                indices.push_back((y + 1) * (segments + 1) + x);
                indices.push_back(y * (segments + 1) + x);
                indices.push_back(y * (segments + 1) + x + 1);

                indices.push_back((y + 1) * (segments + 1) + x);
                indices.push_back(y * (segments + 1) + x + 1);
                indices.push_back((y + 1) * (segments + 1) + x + 1);
            }
        }
    }
}

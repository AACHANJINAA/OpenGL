#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// We need VERTEX definition. Since stdafx.h is included first, VERTEX might not be available yet in header evaluation unless it's defined.
// Let's declare VERTEX here if not already defined, or simply define it as a struct.
#ifndef VERTEX_DEFINED
#define VERTEX_DEFINED
struct VERTEX {
    glm::vec3 _position;
    glm::vec3 _color;
};
#endif

namespace GEOMETRYGENERATOR {
    inline void generate_cube(std::vector<VERTEX>& vertices, std::vector<unsigned int>& indices) {
        vertices = {
            // Front face
            { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) }, // Red
            { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Green
            { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Blue
            { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 0.0f) }, // Yellow

            // Back face
            { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 1.0f) }, // Magenta
            { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 1.0f) }, // Cyan
            { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f) }, // White
            { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.1f, 0.1f, 0.1f) }  // Dark Gray
        };

        indices = {
            0, 1, 2, 2, 3, 0, // Front
            1, 5, 6, 6, 2, 1, // Right
            5, 4, 7, 7, 6, 5, // Back
            4, 0, 3, 3, 7, 4, // Left
            3, 2, 6, 6, 7, 3, // Top
            4, 5, 1, 1, 0, 4  // Bottom
        };
    }

    inline void generate_pyramid(std::vector<VERTEX>& vertices, std::vector<unsigned int>& indices) {
        // A pyramid has 5 vertices (4 base, 1 apex)
        // To assign flat colors per face, we duplicate vertices for each face.
        vertices = {
            // Front face (Apex + Base front-left + Base front-right)
            { glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(1.0f, 0.0f, 0.0f) }, // Apex - Red
            { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },

            // Right face (Apex + Base front-right + Base back-right)
            { glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Apex - Green
            { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },

            // Back face (Apex + Base back-right + Base back-left)
            { glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Apex - Blue
            { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
            { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },

            // Left face (Apex + Base back-left + Base front-left)
            { glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(1.0f, 1.0f, 0.0f) }, // Apex - Yellow
            { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f) },
            { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 0.0f) },

            // Base face (Square base - we split into 2 triangles)
            { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 0.5f) }, // Gray base
            { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 0.5f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f) },
            { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f) }
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

        for (unsigned int y = 0; y <= rings; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)rings;
                float xPos = std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
                float yPos = std::cos(ySegment * glm::pi<float>());
                float zPos = std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

                glm::vec3 pos = glm::vec3(xPos, yPos, zPos) * radius;
                
                // Color gradient based on positions
                glm::vec3 color = glm::vec3(
                    0.5f + 0.5f * xPos,
                    0.5f + 0.5f * yPos,
                    0.5f + 0.5f * zPos
                );

                vertices.push_back({ pos, color });
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

#include "stdafx.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Components/MeshComponent.h"
#include "imgui.h"
#include "CameraManager.h"
#include "ResourceManager.h"
#include "../Core/Shader.h"
#include <algorithm>
#include <cmath>

// Helper structure/function for ray-AABB intersection
namespace {
    bool ray_aabb_intersect(
        const glm::vec3& ray_origin, const glm::vec3& ray_dir,
        const glm::vec3& aabb_min, const glm::vec3& aabb_max,
        float& t_out) 
    {
        float tmin = 0.0f;
        float tmax = 100000.0f;

        for (int i = 0; i < 3; ++i) {
            if (std::abs(ray_dir[i]) < 0.0001f) {
                // Ray is parallel to slab. No hit if origin is not within slab.
                if (ray_origin[i] < aabb_min[i] || ray_origin[i] > aabb_max[i]) {
                    return false;
                }
            } else {
                float ood = 1.0f / ray_dir[i];
                float t1 = (aabb_min[i] - ray_origin[i]) * ood;
                float t2 = (aabb_max[i] - ray_origin[i]) * ood;

                if (t1 > t2) std::swap(t1, t2);

                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);

                if (tmin > tmax) return false;
            }
        }

        t_out = tmin;
        return true;
    }
}

SCENEMANAGER& SCENEMANAGER::get_instance() {
    static SCENEMANAGER instance;
    return instance;
}

std::shared_ptr<GAMEOBJECT> SCENEMANAGER::create_gameobject() {
    auto go = std::make_shared<GAMEOBJECT>();
    _gameobjects.push_back(go);
    return go;
}

void SCENEMANAGER::add_gameobject(const std::shared_ptr<GAMEOBJECT>& go) {
    _gameobjects.push_back(go);
}

void SCENEMANAGER::start() {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->start();
    }
}

void SCENEMANAGER::update(float delta_time) {
    auto& input = INPUTMANAGER::get_instance();

    // 1. Check for Q, W, E, R key presses to change tool mode (if not typing in ImGui)
    if (!ImGui::GetIO().WantTextInput) {
        static bool last_q = false, last_w = false, last_e = false, last_r = false;
        bool curr_q = input.is_key_pressed(GLFW_KEY_Q);
        bool curr_w = input.is_key_pressed(GLFW_KEY_W);
        bool curr_e = input.is_key_pressed(GLFW_KEY_E);
        bool curr_r = input.is_key_pressed(GLFW_KEY_R);

        // QWER tool switching (Only when camera flight - RMB - is not active)
        if (!input.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (curr_q && !last_q) _tool_mode = TOOLMODE::SELECT;
            else if (curr_w && !last_w) _tool_mode = TOOLMODE::TRANSLATE;
            else if (curr_e && !last_e) _tool_mode = TOOLMODE::ROTATE;
            else if (curr_r && !last_r) _tool_mode = TOOLMODE::SCALE;
        }

        last_q = curr_q;
        last_w = curr_w;
        last_e = curr_e;
        last_r = curr_r;
    }

    // 2. Propagate updates to gameobjects
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->update(delta_time);
    }
}

void SCENEMANAGER::late_update(float delta_time) {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->late_update(delta_time);
    }
}

struct GridVertex {
    glm::vec3 pos;
    glm::vec3 color;
};

void SCENEMANAGER::render() {
    static unsigned int grid_vao = 0;
    static unsigned int grid_vbo = 0;
    static int grid_vertex_count = 0;

    if (grid_vao == 0) {
        std::vector<GridVertex> vertices;
        float spacing = 0.1f;         // 10cm grid spacing
        float major_spacing = 1.0f;   // 1m major line spacing
        float range = 10.0f;          // Grid extent from -10 to +10

        // Generate lines parallel to Z-axis (varying X)
        for (float x = -range; x <= range + 0.001f; x += spacing) {
            glm::vec3 color(0.3f, 0.3f, 0.3f); // Minor grid lines (dark gray)
            if (std::abs(x) < 0.0001f) {
                color = glm::vec3(0.2f, 0.4f, 0.8f); // Z-axis line (blue)
            }
            else if (std::fmod(std::abs(x) + 0.0001f, major_spacing) < 0.005f) {
                color = glm::vec3(0.7f, 0.7f, 0.7f); // Major grid lines (light gray)
            }
            vertices.push_back({ glm::vec3(x, 0.0f, -range), color });
            vertices.push_back({ glm::vec3(x, 0.0f, range), color });
        }

        // Generate lines parallel to X-axis (varying Z)
        for (float z = -range; z <= range + 0.001f; z += spacing) {
            glm::vec3 color(0.3f, 0.3f, 0.3f); // Minor grid lines (dark gray)
            if (std::abs(z) < 0.0001f) {
                color = glm::vec3(0.8f, 0.2f, 0.2f); // X-axis line (red)
            }
            else if (std::fmod(std::abs(z) + 0.0001f, major_spacing) < 0.005f) {
                color = glm::vec3(0.7f, 0.7f, 0.7f); // Major grid lines (light gray)
            }
            vertices.push_back({ glm::vec3(-range, 0.0f, z), color });
            vertices.push_back({ glm::vec3(range, 0.0f, z), color });
        }

        grid_vertex_count = static_cast<int>(vertices.size());

        glGenVertexArrays(1, &grid_vao);
        glGenBuffers(1, &grid_vbo);

        glBindVertexArray(grid_vao);
        glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GridVertex), vertices.data(), GL_STATIC_DRAW);

        // Position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)offsetof(GridVertex, pos));
        glEnableVertexAttribArray(0);

        // Color attribute (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)offsetof(GridVertex, color));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Draw Grid Lines first so they sit behind opaque geometry
    auto shader = RESOURCEMANAGER::get_instance().get_shader("cubeShader");
    if (shader) {
        shader->use();
        shader->set_mat4("model", glm::mat4(1.0f));
        
        int win_w, win_h;
        INPUTMANAGER::get_instance().get_window_size(win_w, win_h);
        float aspect = (win_h > 0) ? static_cast<float>(win_w) / static_cast<float>(win_h) : 1.0f;
        
        glm::mat4 view = CAMERAMANAGER::get_instance().get_view_matrix();
        glm::mat4 proj = CAMERAMANAGER::get_instance().get_projection_matrix(aspect);
        
        shader->set_mat4("view", view);
        shader->set_mat4("projection", proj);
        shader->set_float("u_selected", 0.0f); // Make sure grid is not highlighted

        glBindVertexArray(grid_vao);
        glDrawArrays(GL_LINES, 0, grid_vertex_count);
        glBindVertexArray(0);
    }

    // Render GameObjects
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->render();
    }
}

void SCENEMANAGER::end() {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->end();
    }
}

void SCENEMANAGER::clear() {
    _gameobjects.clear();
    _selected_gameobject = nullptr;
    _tool_mode = TOOLMODE::SELECT;
}

void SCENEMANAGER::set_selected_gameobject(const std::shared_ptr<GAMEOBJECT>& go) {
    _selected_gameobject = go;
}

std::shared_ptr<GAMEOBJECT> SCENEMANAGER::get_selected_gameobject() const {
    return _selected_gameobject;
}

std::shared_ptr<GAMEOBJECT> SCENEMANAGER::raycast_closest(const glm::vec3& ray_origin, const glm::vec3& ray_dir) {
    std::shared_ptr<GAMEOBJECT> closest_obj = nullptr;
    float min_t = 100000.0f;

    for (auto& go : _gameobjects) {
        auto mesh = go->get_component<MESHCOMPONENT>();
        auto transform = go->get_component<TRANSFORMCOMPONENT>();
        if (!mesh || !transform) continue;

        // Transform ray to the local space of the GameObject
        glm::mat4 model = transform->get_model_matrix();
        glm::mat4 inv_model = glm::inverse(model);

        glm::vec3 local_ro = glm::vec3(inv_model * glm::vec4(ray_origin, 1.0f));
        glm::vec3 local_rd = glm::normalize(glm::vec3(inv_model * glm::vec4(ray_dir, 0.0f)));

        float t = 0.0f;
        if (ray_aabb_intersect(local_ro, local_rd, mesh->get_aabb_min(), mesh->get_aabb_max(), t)) {
            if (t < min_t) {
                min_t = t;
                closest_obj = go;
            }
        }
    }

    return closest_obj;
}

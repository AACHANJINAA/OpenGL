#include "stdafx.h"
#include "SceneManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Components/MeshComponent.h"
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
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->update(delta_time);
    }
}

void SCENEMANAGER::late_update(float delta_time) {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->late_update(delta_time);
    }
}

void SCENEMANAGER::render() {
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
        glm::vec3 local_rd = glm::vec3(inv_model * glm::vec4(ray_dir, 0.0f));

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

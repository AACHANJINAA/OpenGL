#include "stdafx.h"
#include "EditorPanel.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "../Managers/SceneManager.h"
#include "../Managers/InputManager.h"
#include "../Managers/CameraManager.h"
#include "../Managers/LightManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Components/MeshComponent.h"
#include "Command.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <algorithm>

namespace {
    // Helper function to project 3D World space coordinates to 2D Screen space pixels
    glm::vec3 world_to_screen(const glm::vec3& world_pos, float width, float height) {
        glm::mat4 view = CAMERAMANAGER::get_instance().get_view_matrix();
        glm::mat4 proj = CAMERAMANAGER::get_instance().get_projection_matrix(width / height);
        glm::vec4 clip = proj * view * glm::vec4(world_pos, 1.0f);
        
        // Return off-screen if behind camera
        if (clip.w == 0.0f || clip.z < 0.0f) {
            return glm::vec3(-1.0f);
        }
        
        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        float sx = (ndc.x + 1.0f) * 0.5f * width;
        float sy = (1.0f - ndc.y) * 0.5f * height;
        return glm::vec3(sx, sy, clip.z);
    }
}

EDITORPANEL& EDITORPANEL::get_instance() {
    static EDITORPANEL instance;
    return instance;
}

void EDITORPANEL::update_and_render() {
    // Initialize ImGuizmo for this frame
    ImGuizmo::BeginFrame();

    auto& scene = SCENEMANAGER::get_instance();
    auto& input = INPUTMANAGER::get_instance();
    auto& camera = CAMERAMANAGER::get_instance();
    
    auto selected_list = scene.get_selected_gameobjects();
    auto primary_selected = scene.get_selected_gameobject();

    // Query GLFW window size to compute left 20% panel layouts
    int win_w, win_h;
    input.get_window_size(win_w, win_h);
    float screen_w = static_cast<float>(win_w);
    float screen_h = static_cast<float>(win_h);

    float panel_w = screen_w * 0.20f; // Left panel occupies exactly 20% of the screen width
    float height_toolbox = screen_h * 0.40f;
    float height_hierarchy = screen_h * 0.25f;
    float height_inspector = screen_h * 0.35f;

    // Track mouse hover state over editor UI panels to prevent picking clicks from being absorbed
    static bool is_toolbox_hovered = false;
    static bool is_hierarchy_hovered = false;
    static bool is_inspector_hovered = false;
    bool mouse_over_editor_ui = is_toolbox_hovered || is_hierarchy_hovered || is_inspector_hovered;

    // 1. Handle Box Drag Selection & Mouse Picking
    static bool last_mouse_state = false;
    bool current_mouse_state = input.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT);
    
    ImGuiIO& io = ImGui::GetIO();
    static glm::vec2 drag_start_pos(0.0f);
    static bool is_dragging_box = false;

    // Detect click edge (Press)
    if (current_mouse_state && !last_mouse_state && !mouse_over_editor_ui && !ImGuizmo::IsOver()) {
        drag_start_pos = glm::vec2(io.MousePos.x, io.MousePos.y);
        is_dragging_box = true;
    }

    // Draw selection box in foreground list while dragging
    if (current_mouse_state && is_dragging_box) {
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        ImVec2 p_min = ImVec2(std::min(drag_start_pos.x, io.MousePos.x), std::min(drag_start_pos.y, io.MousePos.y));
        ImVec2 p_max = ImVec2(std::max(drag_start_pos.x, io.MousePos.x), std::max(drag_start_pos.y, io.MousePos.y));
        
        // Draw semi-transparent filled rectangle
        draw_list->AddRectFilled(p_min, p_max, ImColor(0.2f, 0.5f, 0.8f, 0.15f));
        // Draw thin solid border representing the selection box
        draw_list->AddRect(p_min, p_max, ImColor(0.2f, 0.5f, 0.8f, 0.6f), 0.0f, 0, 1.5f);
    }

    // Detect release edge (Perform Box/Picking Selection)
    if (!current_mouse_state && last_mouse_state && is_dragging_box) {
        is_dragging_box = false;
        
        float drag_dist = glm::length(glm::vec2(io.MousePos.x, io.MousePos.y) - drag_start_pos);
        bool shift_held = input.is_key_pressed(GLFW_KEY_LEFT_SHIFT) || input.is_key_pressed(GLFW_KEY_RIGHT_SHIFT);

        if (drag_dist < 5.0f) {
            // A. Single click selection (Picking)
            glm::vec3 ray_ro, ray_rd;
            camera.screen_point_to_ray(drag_start_pos.x, drag_start_pos.y, screen_w, screen_h, ray_ro, ray_rd);

            auto closest = scene.raycast_closest(ray_ro, ray_rd);
            if (closest) {
                if (shift_held) {
                    scene.select_gameobject(closest, false); // Add to selection
                } else {
                    scene.select_gameobject(closest, true);  // Clear others and select
                }
            } else {
                if (!shift_held) {
                    scene.clear_selection();                 // Clicked empty space: clear
                }
            }
        } else {
            // B. Box drag selection
            float min_x = std::min(drag_start_pos.x, io.MousePos.x);
            float max_x = std::max(drag_start_pos.x, io.MousePos.x);
            float min_y = std::min(drag_start_pos.y, io.MousePos.y);
            float max_y = std::max(drag_start_pos.y, io.MousePos.y);

            std::vector<std::shared_ptr<GAMEOBJECT>> boxed_list;
            if (shift_held) {
                boxed_list = scene.get_selected_gameobjects(); // Preserve existing selection
            }

            for (const auto& go : scene.get_gameobjects()) {
                auto transform = go->get_component<TRANSFORMCOMPONENT>();
                if (transform) {
                    glm::vec3 screen_pos = world_to_screen(transform->_position, screen_w, screen_h);
                    // Check if projected screen pos lies inside the drag selection box
                    if (screen_pos.x >= min_x && screen_pos.x <= max_x &&
                        screen_pos.y >= min_y && screen_pos.y <= max_y) {
                        
                        if (std::find(boxed_list.begin(), boxed_list.end(), go) == boxed_list.end()) {
                            boxed_list.push_back(go);
                        }
                    }
                }
            }
            scene.select_gameobjects(boxed_list);
        }
        
        // Re-cache selections locally
        selected_list = scene.get_selected_gameobjects();
        primary_selected = scene.get_selected_gameobject();
    }
    last_mouse_state = current_mouse_state;

    // 2. Render Toolbox Panel (Spawner & Tool Mode & Selected Object values)
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_w, height_toolbox), ImGuiCond_Always);
    ImGui::Begin("Toolbox (Object Spawner)", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    is_toolbox_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    
    // Display active gizmo/tool mode
    ImGui::Text("Gizmo / Tool Mode:");
    auto current_mode = scene.get_tool_mode();
    std::string mode_str = "SELECT (Q)";
    if (current_mode == SCENEMANAGER::TOOLMODE::TRANSLATE) mode_str = "TRANSLATE (W)";
    else if (current_mode == SCENEMANAGER::TOOLMODE::ROTATE) mode_str = "ROTATE (E)";
    else if (current_mode == SCENEMANAGER::TOOLMODE::SCALE) mode_str = "SCALE (R)";
    ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "Active: %s", mode_str.c_str());
    
    ImGui::Separator();

    ImGui::Text("Primitive Spawning:");
    if (ImGui::Button("Spawn Cube")) {
        auto go = scene.create_gameobject();
        go->set_name("Cube " + std::to_string(scene.get_gameobjects().size()));
        go->add_component<TRANSFORMCOMPONENT>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        go->add_component<MESHCOMPONENT>(MESHCOMPONENT::SHAPETYPE::CUBE, "cubeShader");
        go->start();
        scene.push_undo_command(std::make_shared<SPAWNCOMMAND>(go));
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Sphere")) {
        auto go = scene.create_gameobject();
        go->set_name("Sphere " + std::to_string(scene.get_gameobjects().size()));
        go->add_component<TRANSFORMCOMPONENT>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        go->add_component<MESHCOMPONENT>(MESHCOMPONENT::SHAPETYPE::SPHERE, "cubeShader");
        go->start();
        scene.push_undo_command(std::make_shared<SPAWNCOMMAND>(go));
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Pyramid")) {
        auto go = scene.create_gameobject();
        go->set_name("Pyramid " + std::to_string(scene.get_gameobjects().size()));
        go->add_component<TRANSFORMCOMPONENT>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        go->add_component<MESHCOMPONENT>(MESHCOMPONENT::SHAPETYPE::PYRAMID, "cubeShader");
        go->start();
        scene.push_undo_command(std::make_shared<SPAWNCOMMAND>(go));
    }
    
    ImGui::Separator();
    if (ImGui::Button("Clear All GameObjects")) {
        if (!scene.get_gameobjects().empty()) {
            scene.push_undo_command(std::make_shared<DELETECOMMAND>(scene.get_gameobjects()));
        }
        scene.clear();
        selected_list.clear();
        primary_selected = nullptr;
    }

    // Display selected object's numerical values below spawn buttons
    if (!selected_list.empty()) {
        ImGui::Separator();
        if (selected_list.size() == 1 && primary_selected) {
            ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "Selected Object Transform:");
            auto transform = primary_selected->get_component<TRANSFORMCOMPONENT>();
            if (transform) {
                ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform->_position.x, transform->_position.y, transform->_position.z);
                ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform->_rotation.x, transform->_rotation.y, transform->_rotation.z);
                ImGui::Text("Scale:    (%.2f, %.2f, %.2f)", transform->_scale.x, transform->_scale.y, transform->_scale.z);
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "Multiple Objects Selected (%d):", (int)selected_list.size());
            ImGui::Text("Positions and scales differ.");
        }
    } else {
        ImGui::Separator();
        ImGui::Text("No object selected.");
    }

    // Directional Light Settings
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Directional Light Settings")) {
        auto& light = LIGHTMANAGER::get_instance();
        
        // Direction control
        glm::vec3 dir = light.get_direction();
        if (ImGui::DragFloat3("Light Direction", &dir.x, 0.01f, -1.0f, 1.0f)) {
            light.set_direction(dir); // Automatically normalized inside set_direction
        }
        
        // Color control
        glm::vec3 color = light.get_color();
        if (ImGui::ColorEdit3("Light Color", &color.x)) {
            light.set_color(color);
        }
        
        // Intensity control
        float intensity = light.get_intensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.05f, 0.0f, 10.0f)) {
            light.set_intensity(intensity);
        }
        
        // Ambient control
        glm::vec3 ambient = light.get_ambient();
        if (ImGui::ColorEdit3("Ambient Color", &ambient.x)) {
            light.set_ambient(ambient);
        }
    }

    ImGui::End();

    // 3. Render Hierarchy Panel
    ImGui::SetNextWindowPos(ImVec2(0.0f, height_toolbox), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_w, height_hierarchy), ImGuiCond_Always);
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    is_hierarchy_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    auto& list = scene.get_gameobjects();
    for (size_t i = 0; i < list.size(); ++i) {
        std::string label = list[i]->get_name() + "##" + std::to_string(i);
        bool is_selected = scene.is_selected(list[i].get());
        if (ImGui::Selectable(label.c_str(), is_selected)) {
            bool shift_held = input.is_key_pressed(GLFW_KEY_LEFT_SHIFT) || input.is_key_pressed(GLFW_KEY_RIGHT_SHIFT);
            if (shift_held) {
                if (is_selected) {
                    scene.deselect_gameobject(list[i]);
                } else {
                    scene.select_gameobject(list[i], false);
                }
            } else {
                scene.select_gameobject(list[i], true);
            }
            selected_list = scene.get_selected_gameobjects();
            primary_selected = scene.get_selected_gameobject();
        }
    }
    ImGui::End();

    // 4. Render Inspector Panel
    ImGui::SetNextWindowPos(ImVec2(0.0f, height_toolbox + height_hierarchy), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_w, height_inspector), ImGuiCond_Always);
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    is_inspector_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    
    if (!selected_list.empty()) {
        if (selected_list.size() == 1 && primary_selected) {
            // Single Selection Inspector
            char name_buf[64];
            strncpy_s(name_buf, sizeof(name_buf), primary_selected->get_name().c_str(), _TRUNCATE);
            if (ImGui::InputText("Name", name_buf, sizeof(name_buf))) {
                primary_selected->set_name(name_buf);
            }

            ImGui::Separator();

            auto transform = primary_selected->get_component<TRANSFORMCOMPONENT>();
            if (transform) {
                ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Transform Component");
                ImGui::DragFloat3("Position", &transform->_position.x, 0.05f);
                ImGui::DragFloat3("Rotation", &transform->_rotation.x, 1.0f);
                ImGui::DragFloat3("Scale", &transform->_scale.x, 0.05f);
            }

            ImGui::Separator();

            auto mesh = primary_selected->get_component<MESHCOMPONENT>();
            if (mesh) {
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.7f, 1.0f), "Mesh Component");
                std::string shape_name = "Unknown";
                switch (mesh->get_shape_type()) {
                    case MESHCOMPONENT::SHAPETYPE::CUBE: shape_name = "Cube"; break;
                    case MESHCOMPONENT::SHAPETYPE::SPHERE: shape_name = "Sphere"; break;
                    case MESHCOMPONENT::SHAPETYPE::PYRAMID: shape_name = "Pyramid"; break;
                }
                ImGui::Text("Shape type: %s", shape_name.c_str());
                ImGui::Text("Local Bounds:");
                ImGui::Text("  Min: (%.2f, %.2f, %.2f)", mesh->get_aabb_min().x, mesh->get_aabb_min().y, mesh->get_aabb_min().z);
                ImGui::Text("  Max: (%.2f, %.2f, %.2f)", mesh->get_aabb_max().x, mesh->get_aabb_max().y, mesh->get_aabb_max().z);
            }
        } else {
            // Multi Selection Inspector (Delta editing like Unity/Unreal)
            ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Multiple Objects Selected (%d)", (int)selected_list.size());
            ImGui::Separator();

            // We use the primary selected object's transform values as display base.
            // When modified, we compute the delta and apply it to all selected objects.
            auto primary_trans = primary_selected ? primary_selected->get_component<TRANSFORMCOMPONENT>() : nullptr;
            if (primary_trans) {
                // Position Delta
                float pos_val[3] = { primary_trans->_position.x, primary_trans->_position.y, primary_trans->_position.z };
                if (ImGui::DragFloat3("Position (Offset)", pos_val, 0.05f)) {
                    glm::vec3 delta = glm::vec3(pos_val[0], pos_val[1], pos_val[2]) - primary_trans->_position;
                    for (auto& go : selected_list) {
                        auto t = go->get_component<TRANSFORMCOMPONENT>();
                        if (t) t->_position += delta;
                    }
                }

                // Rotation Delta
                float rot_val[3] = { primary_trans->_rotation.x, primary_trans->_rotation.y, primary_trans->_rotation.z };
                if (ImGui::DragFloat3("Rotation (Offset)", rot_val, 1.0f)) {
                    glm::vec3 delta = glm::vec3(rot_val[0], rot_val[1], rot_val[2]) - primary_trans->_rotation;
                    for (auto& go : selected_list) {
                        auto t = go->get_component<TRANSFORMCOMPONENT>();
                        if (t) t->_rotation += delta;
                    }
                }

                // Scale Ratio
                float scl_val[3] = { primary_trans->_scale.x, primary_trans->_scale.y, primary_trans->_scale.z };
                if (ImGui::DragFloat3("Scale (Delta)", scl_val, 0.05f)) {
                    for (int i = 0; i < 3; ++i) {
                        if (scl_val[i] < 0.001f) scl_val[i] = 0.001f;
                    }
                    glm::vec3 old_scl = primary_trans->_scale;
                    glm::vec3 factor = glm::vec3(scl_val[0], scl_val[1], scl_val[2]) / old_scl;
                    for (auto& go : selected_list) {
                        auto t = go->get_component<TRANSFORMCOMPONENT>();
                        if (t) t->_scale *= factor;
                    }
                }
            }
        }
    } else {
        ImGui::Text("Select objects to inspect properties.");
    }
    ImGui::End();

    // 5. Render ImGuizmo 3D Manipulation Gizmos (Support Multiple Selection Group manipulation)
    if (!selected_list.empty()) {
        if (screen_w > 0 && screen_h > 0) {
            // Set up fullscreen transparent window for ImGuizmo
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(screen_w, screen_h));
            ImGui::Begin("##GizmoWindow", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoScrollbar | 
                ImGuiWindowFlags_NoSavedSettings | 
                ImGuiWindowFlags_NoBackground | 
                ImGuiWindowFlags_NoMove);

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(0.0f, 0.0f, screen_w, screen_h);

            float aspect = screen_w / screen_h;
            glm::mat4 view = camera.get_view_matrix();
            glm::mat4 projection = camera.get_projection_matrix(aspect);

            // Compute selection pivot (average position)
            glm::vec3 pivot(0.0f);
            int valid_count = 0;
            for (auto& go : selected_list) {
                auto t = go->get_component<TRANSFORMCOMPONENT>();
                if (t) {
                    pivot += t->_position;
                    valid_count++;
                }
            }
            if (valid_count > 0) {
                pivot /= static_cast<float>(valid_count);
            }

            // Create temporary group transform matrix at the pivot position
            // Group model represents the Gizmo's transform frame
            glm::mat4 group_model = glm::translate(glm::mat4(1.0f), pivot);

            // Convert tool mode to ImGuizmo operations
            ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
            bool draw_gizmo = true;

            switch (scene.get_tool_mode()) {
                case SCENEMANAGER::TOOLMODE::SELECT:
                    draw_gizmo = false;
                    break;
                case SCENEMANAGER::TOOLMODE::TRANSLATE:
                    operation = ImGuizmo::TRANSLATE;
                    break;
                case SCENEMANAGER::TOOLMODE::ROTATE:
                    operation = ImGuizmo::ROTATE;
                    break;
                case SCENEMANAGER::TOOLMODE::SCALE:
                    operation = ImGuizmo::SCALE;
                    break;
            }

            static bool was_using_gizmo = false;
            static std::vector<TransformSnapshot> drag_snapshots;

            if (draw_gizmo) {
                // Drag start: snapshot initial positions
                if (ImGuizmo::IsUsing() && !was_using_gizmo) {
                    drag_snapshots.clear();
                    for (auto& go : selected_list) {
                        auto t = go->get_component<TRANSFORMCOMPONENT>();
                        if (t) {
                            drag_snapshots.push_back({ go, t->_position, t->_rotation, t->_scale });
                        }
                    }
                    was_using_gizmo = true;
                }

                // Manipulate in World Space for natural multi-object pivot interactions
                ImGuizmo::Manipulate(
                    glm::value_ptr(view),
                    glm::value_ptr(projection),
                    operation,
                    ImGuizmo::WORLD, 
                    glm::value_ptr(group_model)
                );

                // Apply translation, rotation, and scale deltas to all selected objects
                if (ImGuizmo::IsUsing()) {
                    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                    ImGuizmo::DecomposeMatrixToComponents(
                        glm::value_ptr(group_model),
                        matrixTranslation,
                        matrixRotation,
                        matrixScale
                    );

                    glm::vec3 new_pivot = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
                    glm::vec3 delta_pos = new_pivot - pivot;
                    
                    glm::vec3 delta_rot = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
                    // Scale factor relative to identity (1,1,1)
                    glm::vec3 delta_scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]) / glm::vec3(1.0f);

                    for (auto& go : selected_list) {
                        auto t = go->get_component<TRANSFORMCOMPONENT>();
                        if (t) {
                            // A. Translate object
                            t->_position += delta_pos;

                            // B. Rotate object locally and orbit around the group pivot
                            if (glm::length(delta_rot) > 0.001f) {
                                t->_rotation += delta_rot;

                                // Orbit position around selection pivot
                                glm::vec3 offset = t->_position - pivot;
                                glm::mat4 rot_matrix = glm::mat4(1.0f);
                                rot_matrix = glm::rotate(rot_matrix, glm::radians(delta_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                                rot_matrix = glm::rotate(rot_matrix, glm::radians(delta_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                                rot_matrix = glm::rotate(rot_matrix, glm::radians(delta_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                                t->_position = pivot + glm::vec3(rot_matrix * glm::vec4(offset, 1.0f));
                            }

                            // C. Scale object
                            t->_scale *= delta_scale;
                        }
                    }
                }
                else {
                    // Drag end: push transformation command to undo stack
                    if (was_using_gizmo) {
                        if (!drag_snapshots.empty()) {
                            scene.push_undo_command(std::make_shared<TRANSFORMCOMMAND>(drag_snapshots));
                        }
                        was_using_gizmo = false;
                        drag_snapshots.clear();
                    }
                }
            }

            ImGui::End(); // End of ##GizmoWindow
        }
    }
}

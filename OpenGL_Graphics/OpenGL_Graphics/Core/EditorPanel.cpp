#include "stdafx.h"
#include "EditorPanel.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "../Managers/SceneManager.h"
#include "../Managers/InputManager.h"
#include "../Managers/CameraManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Components/MeshComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <algorithm>

EDITORPANEL& EDITORPANEL::get_instance() {
    static EDITORPANEL instance;
    return instance;
}

void EDITORPANEL::update_and_render() {
    // Initialize ImGuizmo for this frame
    ImGuizmo::BeginFrame();

    // 1. Handle Mouse Clicking Raycast Selection (Picking)
    static bool last_mouse_state = false;
    bool current_mouse_state = INPUTMANAGER::get_instance().is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT);
    
    // Only raycast if left mouse button was clicked, was not pressed in the previous frame, and ImGui/ImGuizmo doesn't want the mouse.
    // ImGuizmo::IsOver() checks if the mouse is hovering over the 3D translation/rotation/scale handles.
    if (current_mouse_state && !last_mouse_state && !ImGui::GetIO().WantCaptureMouse && !ImGuizmo::IsOver()) {
        double x, y;
        INPUTMANAGER::get_instance().get_mouse_position(x, y);

        int win_w, win_h;
        INPUTMANAGER::get_instance().get_window_size(win_w, win_h);
        float width = static_cast<float>(win_w);
        float height = static_cast<float>(win_h);

        if (width > 0 && height > 0) {
            glm::vec3 ray_ro, ray_rd;
            CAMERAMANAGER::get_instance().screen_point_to_ray(
                static_cast<float>(x), static_cast<float>(y), width, height, ray_ro, ray_rd);

            auto closest = SCENEMANAGER::get_instance().raycast_closest(ray_ro, ray_rd);
            SCENEMANAGER::get_instance().set_selected_gameobject(closest);
        }
    }
    last_mouse_state = current_mouse_state;

    // 2. Render Toolbox Panel (Spawner & Tool Mode)
    ImGui::Begin("Toolbox (Object Spawner)");
    
    // Display active gizmo/tool mode
    ImGui::Text("Gizmo / Tool Mode:");
    auto current_mode = SCENEMANAGER::get_instance().get_tool_mode();
    std::string mode_str = "SELECT (Q)";
    if (current_mode == SCENEMANAGER::TOOLMODE::TRANSLATE) mode_str = "TRANSLATE (W)";
    else if (current_mode == SCENEMANAGER::TOOLMODE::ROTATE) mode_str = "ROTATE (E)";
    else if (current_mode == SCENEMANAGER::TOOLMODE::SCALE) mode_str = "SCALE (R)";
    ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "Active: %s", mode_str.c_str());
    
    ImGui::Separator();

    ImGui::Text("Primitive Spawning:");
    if (ImGui::Button("Spawn Cube")) {
        auto go = SCENEMANAGER::get_instance().create_gameobject();
        go->set_name("Cube " + std::to_string(SCENEMANAGER::get_instance().get_gameobjects().size()));
        go->add_component<TRANSFORMCOMPONENT>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        go->add_component<MESHCOMPONENT>(MESHCOMPONENT::SHAPETYPE::CUBE, "cubeShader");
        go->start();
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Sphere")) {
        auto go = SCENEMANAGER::get_instance().create_gameobject();
        go->set_name("Sphere " + std::to_string(SCENEMANAGER::get_instance().get_gameobjects().size()));
        go->add_component<TRANSFORMCOMPONENT>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        go->add_component<MESHCOMPONENT>(MESHCOMPONENT::SHAPETYPE::SPHERE, "cubeShader");
        go->start();
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn Pyramid")) {
        auto go = SCENEMANAGER::get_instance().create_gameobject();
        go->set_name("Pyramid " + std::to_string(SCENEMANAGER::get_instance().get_gameobjects().size()));
        go->add_component<TRANSFORMCOMPONENT>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        go->add_component<MESHCOMPONENT>(MESHCOMPONENT::SHAPETYPE::PYRAMID, "cubeShader");
        go->start();
    }
    
    ImGui::Separator();
    if (ImGui::Button("Clear All GameObjects")) {
        SCENEMANAGER::get_instance().clear();
    }
    ImGui::End();

    // 3. Render Hierarchy Panel
    ImGui::Begin("Hierarchy");
    auto& list = SCENEMANAGER::get_instance().get_gameobjects();
    auto selected = SCENEMANAGER::get_instance().get_selected_gameobject();
    for (size_t i = 0; i < list.size(); ++i) {
        std::string label = list[i]->get_name() + "##" + std::to_string(i);
        bool is_selected = (list[i] == selected);
        if (ImGui::Selectable(label.c_str(), is_selected)) {
            SCENEMANAGER::get_instance().set_selected_gameobject(list[i]);
        }
    }
    ImGui::End();

    // 4. Render Inspector Panel
    ImGui::Begin("Inspector");
    if (selected) {
        // Name Field
        char name_buf[64];
        strncpy_s(name_buf, sizeof(name_buf), selected->get_name().c_str(), _TRUNCATE);
        if (ImGui::InputText("Name", name_buf, sizeof(name_buf))) {
            selected->set_name(name_buf);
        }

        ImGui::Separator();

        // Transform properties
        auto transform = selected->get_component<TRANSFORMCOMPONENT>();
        if (transform) {
            ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Transform Component");
            ImGui::DragFloat3("Position", &transform->_position.x, 0.05f);
            ImGui::DragFloat3("Rotation", &transform->_rotation.x, 1.0f);
            ImGui::DragFloat3("Scale", &transform->_scale.x, 0.05f);
        }

        ImGui::Separator();

        // Mesh details
        auto mesh = selected->get_component<MESHCOMPONENT>();
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
        ImGui::Text("Select an object to inspect properties.");
    }
    ImGui::End();

    // 5. Render ImGuizmo 3D Manipulation Gizmos (Translation, Rotation, Scale)
    if (selected) {
        auto transform = selected->get_component<TRANSFORMCOMPONENT>();
        if (transform) {
            int win_w, win_h;
            INPUTMANAGER::get_instance().get_window_size(win_w, win_h);
            float width = static_cast<float>(win_w);
            float height = static_cast<float>(win_h);

            if (width > 0 && height > 0) {
                // Set up fullscreen transparent window for ImGuizmo
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(width, height));
                ImGui::Begin("##GizmoWindow", nullptr, 
                    ImGuiWindowFlags_NoTitleBar | 
                    ImGuiWindowFlags_NoResize | 
                    ImGuiWindowFlags_NoScrollbar | 
                    ImGuiWindowFlags_NoSavedSettings | 
                    ImGuiWindowFlags_NoBackground | 
                    ImGuiWindowFlags_NoMove);

                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(0.0f, 0.0f, width, height);

                float aspect = width / height;
                glm::mat4 view = CAMERAMANAGER::get_instance().get_view_matrix();
                glm::mat4 projection = CAMERAMANAGER::get_instance().get_projection_matrix(aspect);
                glm::mat4 model = transform->get_model_matrix();

                // Convert tool mode to ImGuizmo operations
                ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
                bool draw_gizmo = true;

                switch (SCENEMANAGER::get_instance().get_tool_mode()) {
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

                if (draw_gizmo) {
                    // Manipulate the transform matrix in Local Space
                    ImGuizmo::Manipulate(
                        glm::value_ptr(view),
                        glm::value_ptr(projection),
                        operation,
                        ImGuizmo::LOCAL,
                        glm::value_ptr(model)
                    );

                    // Decompose new transform matrix if the user is dragging the gizmo handles
                    if (ImGuizmo::IsUsing()) {
                        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                        ImGuizmo::DecomposeMatrixToComponents(
                            glm::value_ptr(model),
                            matrixTranslation,
                            matrixRotation,
                            matrixScale
                        );

                        transform->_position = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
                        transform->_rotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
                        transform->_scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
                    }
                }

                ImGui::End(); // End of ##GizmoWindow
            }
        }
    }
}

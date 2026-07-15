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

    auto selected = SCENEMANAGER::get_instance().get_selected_gameobject();

    // Query GLFW window size to compute left 20% panel layouts
    int win_w, win_h;
    INPUTMANAGER::get_instance().get_window_size(win_w, win_h);
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

    // 1. Handle Mouse Clicking Raycast Selection (Picking)
    static bool last_mouse_state = false;
    bool current_mouse_state = INPUTMANAGER::get_instance().is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT);
    
    // Perform selection raycast only when Left Mouse Button is clicked (press edge), 
    // the cursor is not over the editor panels, and the cursor is not over the 3D Gizmo handles.
    if (current_mouse_state && !last_mouse_state && !mouse_over_editor_ui && !ImGuizmo::IsOver()) {
        double x, y;
        INPUTMANAGER::get_instance().get_mouse_position(x, y);

        if (screen_w > 0 && screen_h > 0) {
            glm::vec3 ray_ro, ray_rd;
            CAMERAMANAGER::get_instance().screen_point_to_ray(
                static_cast<float>(x), static_cast<float>(y), screen_w, screen_h, ray_ro, ray_rd);

            auto closest = SCENEMANAGER::get_instance().raycast_closest(ray_ro, ray_rd);
            SCENEMANAGER::get_instance().set_selected_gameobject(closest);
            selected = closest; // Update local cache
        }
    }
    last_mouse_state = current_mouse_state;

    // 2. Render Toolbox Panel (Spawner & Tool Mode & Selected Object values)
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_w, height_toolbox), ImGuiCond_Always);
    ImGui::Begin("Toolbox (Object Spawner)", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    is_toolbox_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    
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
        selected = nullptr;
    }

    // Display selected object's numerical values below spawn buttons
    if (selected) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "Selected Object Transform:");
        auto transform = selected->get_component<TRANSFORMCOMPONENT>();
        if (transform) {
            ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform->_position.x, transform->_position.y, transform->_position.z);
            ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform->_rotation.x, transform->_rotation.y, transform->_rotation.z);
            ImGui::Text("Scale:    (%.2f, %.2f, %.2f)", transform->_scale.x, transform->_scale.y, transform->_scale.z);
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
    auto& list = SCENEMANAGER::get_instance().get_gameobjects();
    for (size_t i = 0; i < list.size(); ++i) {
        std::string label = list[i]->get_name() + "##" + std::to_string(i);
        bool is_selected = (list[i] == selected);
        if (ImGui::Selectable(label.c_str(), is_selected)) {
            SCENEMANAGER::get_instance().set_selected_gameobject(list[i]);
            selected = list[i]; // Update local cache
        }
    }
    ImGui::End();

    // 4. Render Inspector Panel
    ImGui::SetNextWindowPos(ImVec2(0.0f, height_toolbox + height_hierarchy), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_w, height_inspector), ImGuiCond_Always);
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    is_inspector_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
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
            if (screen_w > 0 && screen_h > 0) {
                // Set up fullscreen transparent window for ImGuizmo (NoInputs is NOT set, so ImGuizmo receives clicks correctly)
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

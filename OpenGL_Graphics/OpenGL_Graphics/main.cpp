#include "stdafx.h"
#include "Core/Engine.h"
#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Entities/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/KeyboardController.h"

int main() {
    ENGINE engine(1920, 1200, "OpenGL 3D Rainbow Cube (Manager-Component)");

    if (!engine.initialize()) {
        std::cerr << "Engine initialization failed!" << std::endl;
        return -1;
    }

    // 1. Load shader using RESOURCEMANAGER
    RESOURCEMANAGER::get_instance().load_shader("cubeShader", "Shaders/cube.vs", "Shaders/cube.fs");

    // 2. Create GAMEOBJECT using SCENEMANAGER
    auto cube_gameobject = SCENEMANAGER::get_instance().create_gameobject();

    // 3. Add components
    // 3-1. Add TRANSFORMCOMPONENT
    cube_gameobject->add_component<TRANSFORMCOMPONENT>(
        glm::vec3(0.0f, 0.0f, 0.0f),   // Position (center)
        glm::vec3(0.0f, 0.0f, 0.0f),   // Initial rotation (starts flat)
        glm::vec3(1.0f, 1.0f, 1.0f)    // Scale
    );

    // 3-2. Add MESHCOMPONENT
    cube_gameobject->add_component<MESHCOMPONENT>("cubeShader");

    // 3-3. Add KEYBOARDCONTROLLER (manual control with arrow keys and R key, using InputManager internally)
    cube_gameobject->add_component<KEYBOARDCONTROLLER>(2.0f, 90.0f);

    // 4. Run engine main loop
    engine.run();

    return 0;
}

#include "stdafx.h"
#include "GameFramework.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/SceneManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Components/MeshComponent.h"
#include "../Components/KeyboardController.h"

GAMEFRAMEWORK& GAMEFRAMEWORK::get_instance() {
    static GAMEFRAMEWORK instance;
    return instance;
}

bool GAMEFRAMEWORK::initialize(int width, int height, const std::string& title) {
    // 1. Instantiate and initialize ENGINE
    _engine = std::make_unique<ENGINE>(width, height, title);
    if (!_engine->initialize()) {
        std::cerr << "GAMEFRAMEWORK Error: Failed to initialize ENGINE." << std::endl;
        return false;
    }

    // 2. Setup game resources (Shaders, GameObjects, Components)
    // 2-1. Load shader
    RESOURCEMANAGER::get_instance().load_shader("cubeShader", "Shaders/cube.vs", "Shaders/cube.fs");

    // 2-2. Create main Rainbow Cube game object
    auto cube_gameobject = SCENEMANAGER::get_instance().create_gameobject();

    // 2-3. Add components to the cube
    cube_gameobject->add_component<TRANSFORMCOMPONENT>(
        glm::vec3(0.0f, 0.0f, 0.0f),   // Position
        glm::vec3(0.0f, 0.0f, 0.0f),   // Rotation
        glm::vec3(1.0f, 1.0f, 1.0f)    // Scale
    );
    cube_gameobject->add_component<MESHCOMPONENT>("cubeShader");
    cube_gameobject->add_component<KEYBOARDCONTROLLER>(2.0f, 90.0f);

    return true;
}

void GAMEFRAMEWORK::run() {
    if (_engine) {
        _engine->run();
    }
}

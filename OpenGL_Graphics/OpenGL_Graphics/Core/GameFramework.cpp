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

    // 2. Setup game resources (Shaders)
    load_resources();

    return true;
}

void GAMEFRAMEWORK::load_resources() {
    // Compile and cache shaders
    RESOURCEMANAGER::get_instance().load_shader("cubeShader", "Shaders/cube.vs", "Shaders/cube.fs");
}

void GAMEFRAMEWORK::run() {
    if (_engine) 
        _engine->run();
}

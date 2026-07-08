#include "Core/Engine.h"
#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Entities/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include <iostream>

// ROTATORCOMPONENT defined to show the power of the component design pattern.
class ROTATORCOMPONENT : public COMPONENT {
private:
    float _speed_x;
    float _speed_y;

public:
    ROTATORCOMPONENT(float speed_x = 45.0f, float speed_y = 30.0f)
        : _speed_x(speed_x), _speed_y(speed_y) {}

    void update(float delta_time) override {
        auto transform = _owner->get_component<TRANSFORMCOMPONENT>();
        if (transform) {
            transform->_rotation.x += _speed_x * delta_time;
            transform->_rotation.y += _speed_y * delta_time;

            // Reset rotation angles to prevent overflow
            if (transform->_rotation.x > 360.0f) transform->_rotation.x -= 360.0f;
            if (transform->_rotation.y > 360.0f) transform->_rotation.y -= 360.0f;
        }
    }
};

int main() {
    ENGINE engine(800, 600, "OpenGL 3D Rainbow Cube (Manager-Component)");

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
        glm::vec3(20.0f, 30.0f, 0.0f),  // Initial rotation
        glm::vec3(1.0f, 1.0f, 1.0f)    // Scale
    );

    // 3-2. Add MESHCOMPONENT
    cube_gameobject->add_component<MESHCOMPONENT>("cubeShader");

    // 3-3. Add ROTATORCOMPONENT (automatic rotation)
    cube_gameobject->add_component<ROTATORCOMPONENT>(50.0f, 35.0f);

    // 4. Run engine main loop
    engine.run();

    return 0;
}

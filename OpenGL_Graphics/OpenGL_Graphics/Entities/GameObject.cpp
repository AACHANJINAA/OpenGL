#include "GameObject.h"

void GAMEOBJECT::initialize() {
    // Loop with index to prevent issues if elements are added during initialization
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->initialize();
    }
}

void GAMEOBJECT::update(float delta_time) {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->update(delta_time);
    }
}

void GAMEOBJECT::render() {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->render();
    }
}

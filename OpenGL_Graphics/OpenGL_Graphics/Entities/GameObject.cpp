#include "GameObject.h"

void GAME_OBJECT::initialize() {
    // Loop with index to prevent issues if elements are added during initialization
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->initialize();
    }
}

void GAME_OBJECT::update(float delta_time) {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->update(delta_time);
    }
}

void GAME_OBJECT::render() {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->render();
    }
}

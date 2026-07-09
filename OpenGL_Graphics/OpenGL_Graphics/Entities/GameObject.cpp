#include "stdafx.h"
#include "GameObject.h"

void GAMEOBJECT::start() {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->start();
    }
}

void GAMEOBJECT::update(float delta_time) {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->update(delta_time);
    }
}

void GAMEOBJECT::late_update(float delta_time) {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->late_update(delta_time);
    }
}

void GAMEOBJECT::render() {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->render();
    }
}

void GAMEOBJECT::end() {
    for (size_t i = 0; i < _components.size(); ++i) {
        _components[i]->end();
    }
}

#include "SceneManager.h"
#include "../Entities/GameObject.h"

SCENEMANAGER& SCENEMANAGER::get_instance() {
    static SCENEMANAGER instance;
    return instance;
}

std::shared_ptr<GAMEOBJECT> SCENEMANAGER::create_gameobject() {
    auto go = std::make_shared<GAMEOBJECT>();
    _gameobjects.push_back(go);
    return go;
}

void SCENEMANAGER::add_gameobject(const std::shared_ptr<GAMEOBJECT>& go) {
    _gameobjects.push_back(go);
}

void SCENEMANAGER::initialize() {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->initialize();
    }
}

void SCENEMANAGER::update(float delta_time) {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->update(delta_time);
    }
}

void SCENEMANAGER::render() {
    for (size_t i = 0; i < _gameobjects.size(); ++i) {
        _gameobjects[i]->render();
    }
}

void SCENEMANAGER::clear() {
    _gameobjects.clear();
}

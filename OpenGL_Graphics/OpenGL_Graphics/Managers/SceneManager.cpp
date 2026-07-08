#include "SceneManager.h"
#include "../Entities/GameObject.h"

SCENE_MANAGER& SCENE_MANAGER::get_instance() {
    static SCENE_MANAGER instance;
    return instance;
}

std::shared_ptr<GAME_OBJECT> SCENE_MANAGER::create_game_object() {
    auto go = std::make_shared<GAME_OBJECT>();
    _game_objects.push_back(go);
    return go;
}

void SCENE_MANAGER::add_game_object(const std::shared_ptr<GAME_OBJECT>& go) {
    _game_objects.push_back(go);
}

void SCENE_MANAGER::initialize() {
    for (size_t i = 0; i < _game_objects.size(); ++i) {
        _game_objects[i]->initialize();
    }
}

void SCENE_MANAGER::update(float delta_time) {
    for (size_t i = 0; i < _game_objects.size(); ++i) {
        _game_objects[i]->update(delta_time);
    }
}

void SCENE_MANAGER::render() {
    for (size_t i = 0; i < _game_objects.size(); ++i) {
        _game_objects[i]->render();
    }
}

void SCENE_MANAGER::clear() {
    _game_objects.clear();
}

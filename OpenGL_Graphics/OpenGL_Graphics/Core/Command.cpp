#include "stdafx.h"
#include "Command.h"
#include "../Managers/SceneManager.h"
#include "../Entities/GameObject.h"
#include "../Components/TransformComponent.h"
#include <algorithm>

// --- SPAWNCOMMAND ---
SPAWNCOMMAND::SPAWNCOMMAND(const std::shared_ptr<GAMEOBJECT>& go) : _go(go) {}

void SPAWNCOMMAND::undo() {
    auto& scene = SCENEMANAGER::get_instance();
    // Retrieve writable list
    auto& list = const_cast<std::vector<std::shared_ptr<GAMEOBJECT>>&>(scene.get_gameobjects());
    
    auto it = std::find(list.begin(), list.end(), _go);
    if (it != list.end()) {
        _go->end();
        list.erase(it);
    }
    
    scene.deselect_gameobject(_go);
}

// --- DELETECOMMAND ---
DELETECOMMAND::DELETECOMMAND(const std::vector<std::shared_ptr<GAMEOBJECT>>& list) : _deleted_gos(list) {}

void DELETECOMMAND::undo() {
    auto& scene = SCENEMANAGER::get_instance();
    auto& list = const_cast<std::vector<std::shared_ptr<GAMEOBJECT>>&>(scene.get_gameobjects());
    
    for (const auto& go : _deleted_gos) {
        list.push_back(go);
        go->start(); // Re-initialize GL components if necessary
    }
    
    scene.select_gameobjects(_deleted_gos);
}

// --- TRANSFORMCOMMAND ---
TRANSFORMCOMMAND::TRANSFORMCOMMAND(const std::vector<TransformSnapshot>& snapshots) : _snapshots(snapshots) {}

void TRANSFORMCOMMAND::undo() {
    for (const auto& snap : _snapshots) {
        auto t = snap.go->get_component<TRANSFORMCOMPONENT>();
        if (t) {
            t->_position = snap.pos;
            t->_rotation = snap.rot;
            t->_scale = snap.scale;
        }
    }
}

#pragma once
#include <vector>
#include <memory>

class GAMEOBJECT;

class SCENEMANAGER {
private:
    std::vector<std::shared_ptr<GAMEOBJECT>> _gameobjects;

    SCENEMANAGER() = default;
    ~SCENEMANAGER() = default;
    SCENEMANAGER(const SCENEMANAGER&) = delete;
    SCENEMANAGER& operator=(const SCENEMANAGER&) = delete;

public:
    static SCENEMANAGER& get_instance();

    std::shared_ptr<GAMEOBJECT> create_gameobject();
    void add_gameobject(const std::shared_ptr<GAMEOBJECT>& go);

    void start();
    void update(float delta_time);
    void late_update(float delta_time);
    void render();
    void end();
    void clear();
};

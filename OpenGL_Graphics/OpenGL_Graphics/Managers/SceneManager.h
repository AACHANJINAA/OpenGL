#pragma once
#include <vector>
#include <memory>

class GAME_OBJECT;

class SCENE_MANAGER {
private:
    std::vector<std::shared_ptr<GAME_OBJECT>> _game_objects;

    SCENE_MANAGER() = default;
    ~SCENE_MANAGER() = default;
    SCENE_MANAGER(const SCENE_MANAGER&) = delete;
    SCENE_MANAGER& operator=(const SCENE_MANAGER&) = delete;

public:
    static SCENE_MANAGER& get_instance();

    std::shared_ptr<GAME_OBJECT> create_game_object();
    void add_game_object(const std::shared_ptr<GAME_OBJECT>& go);

    void initialize();
    void update(float delta_time);
    void render();
    void clear();
};

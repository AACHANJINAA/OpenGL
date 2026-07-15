#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class GAMEOBJECT;

class SCENEMANAGER {
public:
    enum class TOOLMODE {
        SELECT,
        TRANSLATE,
        ROTATE,
        SCALE
    };

private:
    std::vector<std::shared_ptr<GAMEOBJECT>> _gameobjects;
    std::shared_ptr<GAMEOBJECT> _selected_gameobject = nullptr;
    TOOLMODE _tool_mode = TOOLMODE::SELECT;

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

    const std::vector<std::shared_ptr<GAMEOBJECT>>& get_gameobjects() const { return _gameobjects; }

    void set_selected_gameobject(const std::shared_ptr<GAMEOBJECT>& go);
    std::shared_ptr<GAMEOBJECT> get_selected_gameobject() const;

    std::shared_ptr<GAMEOBJECT> raycast_closest(const glm::vec3& ray_origin, const glm::vec3& ray_dir);

    void set_tool_mode(TOOLMODE mode) { _tool_mode = mode; }
    TOOLMODE get_tool_mode() const { return _tool_mode; }
};

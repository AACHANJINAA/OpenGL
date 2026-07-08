#pragma once

class GAME_OBJECT;

class COMPONENT {
protected:
    GAME_OBJECT* _owner = nullptr;

public:
    virtual ~COMPONENT() = default;
    
    virtual void initialize() {}
    virtual void update(float delta_time) {}
    virtual void render() {}

    void set_owner(GAME_OBJECT* go) { _owner = go; }
    GAME_OBJECT* get_owner() const { return _owner; }
};

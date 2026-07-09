#pragma once

class GAMEOBJECT;

class COMPONENT {
protected:
    GAMEOBJECT* _owner = nullptr;

public:
    virtual ~COMPONENT() = default;
    
    virtual void start() {}
    virtual void update(float delta_time) {}
    virtual void late_update(float delta_time) {}
    virtual void render() {}
    virtual void end() {}

    void set_owner(GAMEOBJECT* go) { _owner = go; }
    GAMEOBJECT* get_owner() const { return _owner; }
};

#pragma once

class GAMEOBJECT;

class COMPONENT {
protected:
    GAMEOBJECT* _owner = nullptr;

public:
    virtual ~COMPONENT() = default;
    
    virtual void initialize() {}
    virtual void update(float delta_time) {}
    virtual void render() {}

    void set_owner(GAMEOBJECT* go) { _owner = go; }
    GAMEOBJECT* get_owner() const { return _owner; }
};

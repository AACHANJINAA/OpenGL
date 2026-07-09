#pragma once
#include "Component.h"

class KEYBOARDCONTROLLER : public COMPONENT {
private:
    float _move_speed;
    float _rotate_speed;

public:
    KEYBOARDCONTROLLER(float move_speed = 2.0f, float rotate_speed = 90.0f);

    void update(float delta_time) override;
};

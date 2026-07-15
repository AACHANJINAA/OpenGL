#include "stdafx.h"
#include "LightManager.h"

LIGHTMANAGER::LIGHTMANAGER()
    // Default direction points along the negative X-axis (shining from the right side to the left side)
    : _dir_light_direction(-1.0f, 0.0f, 0.0f)
    , _dir_light_color(0.3f, 0.4f, 0.3f) // Default directional light color
    , _dir_light_intensity(1.0f)
    , _ambient_light(1.0f, 1.0f, 1.0f) {}

LIGHTMANAGER& LIGHTMANAGER::get_instance() {
    static LIGHTMANAGER instance;
    return instance;
}

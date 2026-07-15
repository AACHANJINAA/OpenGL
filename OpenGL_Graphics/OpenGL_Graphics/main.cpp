#include "stdafx.h"
#include "Core/GameFramework.h"

int main() {
    auto& app = GAMEFRAMEWORK::get_instance();

    if (!app.initialize(1920, 1080, "OpenGL Renderer")) {
        return -1;
    }

    app.run();

    return 0;
}

#pragma once
#include "Engine.h"

class GAMEFRAMEWORK {
private:
    std::unique_ptr<ENGINE> _engine;

    GAMEFRAMEWORK() = default;
    ~GAMEFRAMEWORK() = default;
    GAMEFRAMEWORK(const GAMEFRAMEWORK&) = delete;
    GAMEFRAMEWORK& operator=(const GAMEFRAMEWORK&) = delete;

public:
    static GAMEFRAMEWORK& get_instance();

    bool initialize(int width, int height, const std::string& title);
    void set_cube();
    void run();
};

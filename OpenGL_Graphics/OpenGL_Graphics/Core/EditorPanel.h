#pragma once

class EDITORPANEL {
private:
    EDITORPANEL() = default;
    ~EDITORPANEL() = default;
    EDITORPANEL(const EDITORPANEL&) = delete;
    EDITORPANEL& operator=(const EDITORPANEL&) = delete;

public:
    static EDITORPANEL& get_instance();

    void update_and_render();
};

#include "ResourceManager.h"
#include "../Core/Shader.h"
#include <iostream>

RESOURCE_MANAGER& RESOURCE_MANAGER::get_instance() {
    static RESOURCE_MANAGER instance;
    return instance;
}

std::shared_ptr<SHADER> RESOURCE_MANAGER::load_shader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path) {
    auto shader = std::make_shared<SHADER>(vertex_path, fragment_path);
    _shaders[name] = shader;
    return shader;
}

std::shared_ptr<SHADER> RESOURCE_MANAGER::get_shader(const std::string& name) {
    auto it = _shaders.find(name);
    if (it != _shaders.end()) {
        return it->second;
    }
    return nullptr;
}

void RESOURCE_MANAGER::clear() {
    _shaders.clear();
}

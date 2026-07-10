#include "stdafx.h"
#include "ResourceManager.h"
#include "../Core/Shader.h"

RESOURCEMANAGER& RESOURCEMANAGER::get_instance() {
    static RESOURCEMANAGER instance;
    return instance;
}

std::shared_ptr<SHADER> RESOURCEMANAGER::load_shader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path) {
    auto shader = std::make_shared<SHADER>(vertex_path, fragment_path);
    _shaders[name] = shader;
    return shader;
}

std::shared_ptr<SHADER> RESOURCEMANAGER::get_shader(const std::string& name) {
    auto it = _shaders.find(name);
    if (it != _shaders.end()) {
        return it->second;
    }
    return nullptr;
}

void RESOURCEMANAGER::clear() {

}

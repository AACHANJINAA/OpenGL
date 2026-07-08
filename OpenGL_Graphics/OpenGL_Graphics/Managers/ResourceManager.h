#pragma once
#include <map>
#include <string>
#include <memory>

class SHADER;

class RESOURCE_MANAGER {
private:
    std::map<std::string, std::shared_ptr<SHADER>> _shaders;

    RESOURCE_MANAGER() = default;
    ~RESOURCE_MANAGER() = default;
    RESOURCE_MANAGER(const RESOURCE_MANAGER&) = delete;
    RESOURCE_MANAGER& operator=(const RESOURCE_MANAGER&) = delete;

public:
    static RESOURCE_MANAGER& get_instance();

    std::shared_ptr<SHADER> load_shader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path);
    std::shared_ptr<SHADER> get_shader(const std::string& name);
    void clear();
};

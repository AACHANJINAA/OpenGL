#pragma once
#include <map>
#include <string>
#include <memory>

class SHADER;

class RESOURCEMANAGER {
private:
    std::map<std::string, std::shared_ptr<SHADER>> _shaders;

    RESOURCEMANAGER() = default;
    ~RESOURCEMANAGER() = default;
    RESOURCEMANAGER(const RESOURCEMANAGER&) = delete;
    RESOURCEMANAGER& operator=(const RESOURCEMANAGER&) = delete;

public:
    static RESOURCEMANAGER& get_instance();

    std::shared_ptr<SHADER> load_shader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path);
    std::shared_ptr<SHADER> get_shader(const std::string& name);
    void clear();
};

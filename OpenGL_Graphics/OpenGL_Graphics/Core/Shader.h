#pragma once
#include <string>
#include <glm/glm.hpp>

class SHADER {
public:
    unsigned int _id = 0;

    SHADER(const std::string& vertex_path, const std::string& fragment_path);
    ~SHADER();

    void use() const;

    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_mat4(const std::string& name, const glm::mat4& mat) const;

private:
    void check_compile_errors(unsigned int shader, const std::string& type);
};

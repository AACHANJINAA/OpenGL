#include "Shader.h"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

SHADER::SHADER(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        v_shader_file.open(vertex_path);
        f_shader_file.open(fragment_path);
        std::stringstream v_shader_stream, f_shader_stream;
        
        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();

        v_shader_file.close();
        f_shader_file.close();

        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: Vertex=" << vertex_path << ", Fragment=" << fragment_path << "\n" << e.what() << std::endl;
    }

    const char* v_shader_code_ptr = vertex_code.c_str();
    const char* f_shader_code_ptr = fragment_code.c_str();

    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code_ptr, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code_ptr, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    _id = glCreateProgram();
    glAttachShader(_id, vertex);
    glAttachShader(_id, fragment);
    glLinkProgram(_id);
    check_compile_errors(_id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

SHADER::~SHADER() {
    if (_id != 0) {
        glDeleteProgram(_id);
    }
}

void SHADER::use() const {
    glUseProgram(_id);
}

void SHADER::set_bool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
}

void SHADER::set_int(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void SHADER::set_float(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

void SHADER::set_vec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
}

void SHADER::set_mat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void SHADER::check_compile_errors(unsigned int shader, const std::string& type) {
    int success;
    char info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

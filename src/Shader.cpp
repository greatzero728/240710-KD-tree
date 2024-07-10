/**
 * @file Shader.cpp
 * @brief Implementation of the Shader class
 * @date 21/5/2024
 */

#include "Shader.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

namespace CS350 {

    const char* const Shader::vertexShaderSource = R"(
               #version 330 core
        layout(location = 0) in vec3 attr_position;
        uniform mat4 uniform_mvp;
        uniform mat4 uniform_vp;
        uniform mat4 uniform_m2w;
        out vec4 position;
        void main()
        {
            vec4 vertex = vec4(attr_position, 1.0f);
            position = uniform_vp * uniform_m2w * vertex;
            gl_Position = uniform_mvp * vertex;
        }

    )";

    const char* const Shader::fragmentShaderSource = R"(
        #version 330 core
        in vec4 position;
        out vec4 out_color;
        uniform vec4 uniform_color;
        void main()
        {
            out_color = uniform_color;
        }
    )";

    Shader::Shader() : m_ProgramID(0) {

    }

    Shader::~Shader() {
        glDeleteProgram(m_ProgramID);
    }

    void Shader::CompileAndLinkShaders() {
        // Compile vertex shader
        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderID, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShaderID);
        CheckCompileErrors(vertexShaderID, "VERTEX");

        // Compile fragment shader
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShaderID);
        CheckCompileErrors(fragmentShaderID, "FRAGMENT");

        // Link shaders into a shader program
        m_ProgramID = glCreateProgram();
        glAttachShader(m_ProgramID, vertexShaderID);
        glAttachShader(m_ProgramID, fragmentShaderID);
        glLinkProgram(m_ProgramID);
        CheckLinkErrors(m_ProgramID);

        // Delete the shaders as they're linked into our program now and no longer needed
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    void Shader::Use() const {
        glUseProgram(m_ProgramID);
    }

    void Shader::SetUniform(const std::string& name, int value) const {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetUniform(const std::string& name, float value) const {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetUniform(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    GLint Shader::GetUniformLocation(const std::string& name) const {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end()) {
            return it->second;
        }

        GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
        if (location == -1) {
            std::cerr << "Uniform '" << name << "' not found in shader program." << std::endl;
        }

        m_UniformLocations[name] = location;
        return location;
    }

    void Shader::CheckCompileErrors(GLuint shaderID, const std::string& type) {
        GLint success = 0;
        char infoLog[512];
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }

    void Shader::CheckLinkErrors(GLuint programID) {
        GLint success = 0;
        char infoLog[512];
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }



} // namespace CS350

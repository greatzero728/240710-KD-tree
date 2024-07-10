#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace CS350 {

    class Shader {
    public:
        Shader();
        ~Shader();

        void CompileAndLinkShaders();
        void Use() const;

        void SetUniform(const std::string& name, int value) const;
        void SetUniform(const std::string& name, float value) const;
        void SetUniform(const std::string& name, const glm::vec3& value) const;
        void SetUniform(const std::string& name, const glm::vec4& value) const;
        void SetUniform(const std::string& name, const glm::mat4& value) const;

        GLuint GetProgramID() const { return m_ProgramID; }
        GLint GetUniformLocation(const std::string& name) const;

    private:
        GLuint m_ProgramID;
        mutable std::unordered_map<std::string, GLint> m_UniformLocations;

        void CheckCompileErrors(GLuint shaderID, const std::string& type);
        void CheckLinkErrors(GLuint programID);

        static const char* const vertexShaderSource;
        static const char* const fragmentShaderSource;


    };

} // namespace CS350

#endif // SHADER_HPP

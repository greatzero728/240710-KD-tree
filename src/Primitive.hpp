/**
 * @file DebugRenderer.cpp
 * @brief Declaration of the Debug Renderer class
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 21/5/2024
 * @copyright Copyright (C) 2024 DigiPen Institute of Technology.
 */



#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> 
#include "Shader.hpp"

namespace CS350 {

    class Primitive {
    public:
        Primitive();
        ~Primitive();

        Primitive(const Primitive&) = delete;
        Primitive& operator=(const Primitive&) = delete;

        void Bind() const;
        static void Unbind();
        void Draw(GLenum mode) const;
        void DrawPart(GLenum mode, GLint first, GLsizei count) const;
        GLuint GetVAO() const;
        GLuint GetEBO() const;


        void SetupBuffer(const std::vector<glm::vec3>& vertices);
        void SetupTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
        void SetupPlane(const glm::vec3& position, const glm::vec3& normal, float size);
        GLuint GetVertexCount() const { return m_VertexCount; } // Public function to access m_VertexCount
        void SetupSphere(const glm::vec3& centerPosition, float radius, float discRadius, const glm::vec3& cameraPosition);
        static void CreateSphereVertices(std::vector<glm::vec3>& xyDiscVertices,
            std::vector<glm::vec3>& xzDiscVertices,
            std::vector<glm::vec3>& yzDiscVertices,
            const glm::vec3& centerPosition, float discRadius);
        /* void CreateFrustumVertices(std::vector<glm::vec3>& vertices, const glm::mat4& invViewProj);
         void  SetupFrustum(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices);*/

    private:
        GLuint m_VAO, m_VBO, m_IBO;
        GLuint m_EBO;
        GLsizei m_IndexCount;
        GLsizei m_VertexCount;
        GLuint m_Shader;


    };

} // namespace CS350

#endif // PRIMITIVE_HPP

/**
 * @file DebugRenderer.cpp
 * @brief Defination of the Debug Renderer class
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 21/5/2024
 * @copyright Copyright (C) 2024 DigiPen Institute of Technology.
 */

#include "DebugRenderer.hpp"
#include "Math.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <Shader.hpp>
#include <variant>

namespace CS350 {


    DebugRenderer::DebugRenderer() {
        // Create and compile the shader program
        mShader.CompileAndLinkShaders();
    }

    DebugRenderer::~DebugRenderer() {
        // Clean up is handled by Shader destructor
    }

    void DebugRenderer::ActivateShader() const {
        mShader.Use();
    }

    void DebugRenderer::DeactivateShader() {
        glUseProgram(0);
    }

    void DebugRenderer::DrawPointImmediate(const glm::mat4& viewProjection, const glm::vec3& position, const glm::vec4& color) const {
        ActivateShader();

        // Set up the projection matrix
        glUniformMatrix4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp"), 1, GL_FALSE, glm::value_ptr(viewProjection));

        // Set up the point color
        glUniform4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_color"), 1, glm::value_ptr(color));

        // Create a temporary VAO and VBO
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Set vertex data
        std::vector<glm::vec3> vertices = { position };
        GLsizei dataSize = static_cast<GLsizei>(vertices.size() * sizeof(glm::vec3));
        glBufferData(GL_ARRAY_BUFFER, dataSize, vertices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Draw the point
        glDrawArrays(GL_POINTS, 0, 1);

        // Clean up VAO and VBO
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        DeactivateShader();
    }



    void DebugRenderer::DrawPointImmediate(const glm::mat4& viewProjection, const glm::vec3& position, const glm::vec4& color, float size) const {
        ActivateShader();

        // Create VAO and VBO
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Set vertex data
        std::vector<glm::vec3> vertices = { position };
        // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
         // Calculate the size of the data
        GLsizei dataSize = static_cast<GLsizei>(vertices.size() * sizeof(glm::vec3));
        // Use GLsizei for OpenGL buffer size
        glBufferData(GL_ARRAY_BUFFER, dataSize, vertices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Set shader uniforms
        glUniformMatrix4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp"), 1, GL_FALSE, glm::value_ptr(viewProjection));
        glUniform4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_color"), 1, glm::value_ptr(color));

        // Set point size
        glPointSize(size);

        // Draw the point
        glDrawArrays(GL_POINTS, 0, 1);

        // Clean up VAO and VBO
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        DeactivateShader(); // Deactivate the shader if necessary
    }



    void DebugRenderer::CheckCompileErrors(GLuint shaderID, const std::string& type) {
        GLint success = 0;
        char infoLog[512];
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }


    void DebugRenderer::CheckLinkErrors(GLuint programID) {
        GLint success = 0;
        char infoLog[512];
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (success == 0) {
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }


    void DebugRenderer::DrawSegmentImmediate(const glm::mat4& viewProjection, const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
        ActivateShader();

        // Create vertex data for the line segment
        std::vector<glm::vec3> vertices = { start, end };

        // Setup buffers using Primitive helper function
        mPrimitive.SetupBuffer(vertices);

        // Set the uniform variables in the shader
        glUniformMatrix4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp"), 1, GL_FALSE, glm::value_ptr(viewProjection));
        glUniform4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_color"), 1, glm::value_ptr(color));

        // Draw the line segment using Primitive helper function
        mPrimitive.Draw(GL_LINES);
    }

    void DebugRenderer::DrawTriangleImmediate(const glm::mat4& viewProjection, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) const {
        ActivateShader();

        // Create a Primitive instance
        CS350::Primitive trianglePrimitive;

        // Setup the triangle in the Primitive
        trianglePrimitive.SetupBuffer({ v0, v1, v2 });

        // Set the uniform variables in the shader
        GLint mvpLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
        GLint colorLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_color");
        glUniform4fv(colorLocation, 1, glm::value_ptr(color));

        // Draw the triangle from the Primitive
        trianglePrimitive.Draw(GL_TRIANGLES);

        DeactivateShader();
    }


    void DebugRenderer::DrawAabbImmediate(const glm::mat4& vp, const glm::vec3& centre, const glm::vec3& size, const glm::vec4& color) const {

        static const std::vector<glm::vec3> Vertices = {
            {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}
        };

        static const std::vector<GLuint> Indices = {
            0, 1, 2, 2, 3, 0, // Front face
            4, 5, 6, 6, 7, 4, // Back face
            0, 3, 7, 7, 4, 0, // Left face
            1, 5, 6, 6, 2, 1, // Right face
            3, 2, 6, 6, 7, 3, // Top face
            0, 1, 5, 5, 4, 0  // Bottom face
        };

        static const std::vector<GLuint> wireframeIndices = {
            0, 1, 1, 2, 2, 3, 3, 0, // Front face edges
            4, 5, 5, 6, 6, 7, 7, 4, // Back face edges
            0, 4, 1, 5, 2, 6, 3, 7  // Side edges
        };

        glm::mat4 model = glm::translate(glm::mat4(1.0f), centre) * glm::scale(glm::mat4(1.0f), size / 2.0f);
        glm::mat4 mvp = vp * model;

        // First render the solid cube
        glUseProgram(mShader.GetProgramID());

        glUniformMatrix4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_color"), 1, glm::value_ptr(color));

        glDisable(GL_CULL_FACE);

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec3), Vertices.data(), GL_STATIC_DRAW);
        // Calculate the size of the data
        GLsizei dataSize = static_cast<GLsizei>(Vertices.size() * sizeof(glm::vec3));
        // Use GLsizei for OpenGL buffer size
        glBufferData(GL_ARRAY_BUFFER, dataSize, Vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        // Calculate the size of the data
        GLsizei indicesDataSize = static_cast<GLsizei>(Indices.size() * sizeof(GLuint));
        // Use GLsizei for OpenGL buffer size
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesDataSize, Indices.data(), GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, 0);


        // Clean up the buffers for solid cube
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);

        // Now render the wireframe on top
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec3), Vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(Vertices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), Vertices.data(), GL_STATIC_DRAW);


        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireframeIndices.size() * sizeof(GLuint), wireframeIndices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(wireframeIndices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), wireframeIndices.data(), GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Use black color for wireframe
        glm::vec4 wireframeColor = glm::vec4(0, 0, 0, 1);
        glUniform4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_color"), 1, glm::value_ptr(wireframeColor));

        glDrawElements(GL_LINES, static_cast<GLsizei>(wireframeIndices.size()), GL_UNSIGNED_INT, 0);

        // Clean up the buffers for wireframe
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);

        glUseProgram(0);

        glEnable(GL_CULL_FACE);
    }



    void DebugRenderer::DrawAabbWireframeImmediate(const glm::mat4& viewProjection, const glm::vec3& center, const glm::vec3& size, const glm::vec4& color)const {
        ActivateShader();

        // Calculate half extents
        glm::vec3 halfSize = size * 0.5f;

        // Calculate AABB vertices
        std::vector<glm::vec3> vertices = {
            center + glm::vec3(-halfSize[0], -halfSize[1], -halfSize[2]),  // Front bottom left
            center + glm::vec3(halfSize[0], -halfSize[1], -halfSize[2]),   // Front bottom right
            center + glm::vec3(halfSize[0], halfSize[1], -halfSize[2]),    // Front top right
            center + glm::vec3(-halfSize[0], halfSize[1], -halfSize[2]),   // Front top left
            center + glm::vec3(-halfSize[0], -halfSize[1], halfSize[2]),   // Back bottom left
            center + glm::vec3(halfSize[0], -halfSize[1], halfSize[2]),    // Back bottom right
            center + glm::vec3(halfSize[0], halfSize[1], halfSize[2]),     // Back top right
            center + glm::vec3(-halfSize[0], halfSize[1], halfSize[2])     // Back top left
        };

        // Create indices for drawing wireframe
        std::vector<GLuint> indices = {
            // Front face
            0, 1, 1, 2, 2, 3, 3, 0,

            // Back face
            4, 5, 5, 6, 6, 7, 7, 4,

            // Connect front and back faces
            0, 4, 1, 5, 2, 6, 3, 7
        };

        // Create a vertex array object (VAO) to hold the vertex buffer object (VBO)
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create a vertex buffer object (VBO) to store vertex data
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Calculate the size of the data
        GLsizei dataSize = static_cast<GLsizei>(vertices.size() * sizeof(glm::vec3));
        // Use GLsizei for OpenGL buffer size
        glBufferData(GL_ARRAY_BUFFER, dataSize, vertices.data(), GL_STATIC_DRAW);


        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Create an element buffer object (EBO) to store indices
        GLuint ebo = 0;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), indices.data(), GL_STATIC_DRAW);


        // Set the uniform variables in the shader
        GLint mvpLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
        GLint colorLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_color");
        glUniform4fv(colorLocation, 1, glm::value_ptr(color));

        // Draw the AABB wireframe
        glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);


        // Clean up
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        DeactivateShader();
    }

    void DebugRenderer::DrawPlaneImmediate(const glm::mat4& viewProjection, const glm::vec3& position, const glm::vec3& normal, float size, const glm::vec4& color) const {
        // Create a Primitive instance
        Primitive planePrimitive;

        // Set up the plane with the given parameters
        planePrimitive.SetupPlane(position, normal, (size * 0.5f));

        glUseProgram(mShader.GetProgramID());

        // Set the uniform variables in the shader
        GLint mvpLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
        GLint colorLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_color");
        glUniform4fv(colorLocation, 1, glm::value_ptr(color));

        // Draw the plane
        planePrimitive.Bind();
        //  glDrawElements(GL_TRIANGLES, planePrimitive.GetVertexCount(), GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(planePrimitive.GetVertexCount()), GL_UNSIGNED_INT, 0);


        Primitive::Unbind();




        glm::vec3 normalEnd = position + normal * size;
        std::vector<glm::vec3> normalVertices = { position, normalEnd };

        GLuint normalVao = 0;
        GLuint normalVbo = 0;

        glGenVertexArrays(1, &normalVao);
        glGenBuffers(1, &normalVbo);

        glBindVertexArray(normalVao);

        glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
        //  glBufferData(GL_ARRAY_BUFFER, normalVertices.size() * sizeof(glm::vec3), normalVertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(normalVertices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), normalVertices.data(), GL_STATIC_DRAW);


        // Set vertex attribute pointers for the normal
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Draw the normal line
        glUniform4fv(colorLocation, 1, glm::value_ptr(glm::vec4(1, 1, 1, 1))); // Draw normal in white
        glDrawArrays(GL_LINES, 0, 2);

        // Clean up
        glDeleteBuffers(1, &normalVbo);
        glDeleteVertexArrays(1, &normalVao);
    }


    void DebugRenderer::DrawSphereImmediate(const glm::mat4& viewProjection, const glm::vec3& cameraPosition, const glm::vec3& centerPosition, float radius, const glm::vec4& color) const {
        Primitive spherePrimitive;

        spherePrimitive.SetupSphere(centerPosition, radius, radius * 0.9f, cameraPosition);

        glUseProgram(mShader.GetProgramID());

        GLint mvpLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
        GLint colorLocation = glGetUniformLocation(mShader.GetProgramID(), "uniform_color");
        glUniform4fv(colorLocation, 1, glm::value_ptr(color));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(spherePrimitive.GetVAO());

        glDrawArrays(GL_LINE_LOOP, 0, 37); // XY plane
        glDrawArrays(GL_LINE_LOOP, 37, 37); // XZ plane
        glDrawArrays(GL_LINE_LOOP, 74, 37); // YZ plane

        // Calculate the starting index for the horizontal disc
        int horizontalDiscStartIndex = 37 * 3; // Total vertices of XY, XZ, and YZ planes

        glDrawArrays(GL_LINE_LOOP, horizontalDiscStartIndex, 37); // Horizontal plane

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);
    }



    void SetShaderUniforms(GLuint shaderProgram, const glm::mat4& viewProjection, const glm::vec4& color) {
        GLint vpLoc = glGetUniformLocation(shaderProgram, "uniform_mvp");
        if (vpLoc != -1) {
            glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
        }
        else {
            std::cerr << "Uniform 'uniform_mvp' not found in shader program." << std::endl;
        }

        GLint colorLoc = glGetUniformLocation(shaderProgram, "uniform_color");
        if (colorLoc != -1) {
            glUniform4fv(colorLoc, 1, glm::value_ptr(color));
        }
        else {
            std::cerr << "Uniform 'uniform_color' not found in shader program." << std::endl;
        }
    }

    void DebugRenderer::DrawFrustumImmediate(const glm::mat4& viewProj, const glm::mat4& frustumVP, const glm::vec4& color) const {

        // Extract frustum corners
        std::vector<glm::vec3> corners(8);
        glm::mat4 invVP = glm::inverse(frustumVP);
        std::vector<glm::vec4> ndcCorners = {
            glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
            glm::vec4(1, 1, -1, 1), glm::vec4(-1, 1, -1, 1),
            glm::vec4(-1, -1, 1, 1), glm::vec4(1, -1, 1, 1),
            glm::vec4(1, 1, 1, 1), glm::vec4(-1, 1, 1, 1)
        };

        for (int i = 0; i < 8; ++i) {
            glm::vec4 worldPos = invVP * ndcCorners[i];
            corners[i] = glm::vec3(worldPos) / worldPos.w;
        }

        // Indices for drawing the frustum as a solid object (using triangles)
        std::vector<GLuint> indices = {

            0, 1, 2, 2, 3, 0,  // Near face
            4, 5, 6, 6, 7, 4,  // Far face
            0, 3, 7, 7, 4, 0,  // Left face
            1, 2, 6, 6, 5, 1,  // Right face
            3, 2, 6, 6, 7, 3,  // Top face
            0, 1, 5, 5, 4, 0   // Bottom face
        };

        // Edges of the frustum for outlining
        std::vector<GLuint> edgeIndices = {
            0, 1, 1, 2, 2, 3, 3, 0,    // Near face
            4, 5, 5, 6, 6, 7, 7, 4,    // Far face
            0, 4, 1, 5, 2, 6, 3, 7    // Connecting edges
        };

        // Activate the shader program
        glUseProgram(mShader.GetProgramID());

        // Set shader uniforms
        SetShaderUniforms(mShader.GetProgramID(), viewProj, color);

        // Ensure correct OpenGL state
        glDepthMask(GL_FALSE);   // No Depth-Writing
        glDisable(GL_CULL_FACE); // Disable Culling
        glEnable(GL_BLEND);      // Enable Blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;

        if (vao == 0) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // Vertex attributes
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }
        // Bind the VAO
        glBindVertexArray(vao);

        // Upload frustum vertices and indices to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glBufferData(GL_ARRAY_BUFFER, corners.size() * sizeof(glm::vec3), corners.data(), GL_DYNAMIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, corners.size() * sizeof(glm::vec3), corners.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(corners.size() * sizeof(glm::vec3)), corners.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        //  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);
         // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);


        glm::vec4 edgeColor(0, 0, 0, 1); // Black color for edges
        SetShaderUniforms(mShader.GetProgramID(), viewProj, edgeColor);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndices.size() * sizeof(GLuint), edgeIndices.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(edgeIndices.size()) * static_cast<GLsizeiptr>(sizeof(GLuint)), edgeIndices.data(), GL_DYNAMIC_DRAW);

        glDrawElements(GL_LINES, static_cast<GLsizei>(edgeIndices.size()), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // Deactivate the shader program
        glUseProgram(0);
    }


    void DebugRenderer::DrawFrustumWireframeImmediate(const glm::mat4& viewProj, const glm::mat4& frustumVP, const glm::vec4& color)const {
        // Extract frustum corners

        std::vector<glm::vec3> corners(8);
        glm::mat4 invVP = glm::inverse(frustumVP);
        std::vector<glm::vec4> ndcCorners = {
            glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
            glm::vec4(1, 1, -1, 1), glm::vec4(-1, 1, -1, 1),
            glm::vec4(-1, -1, 1, 1), glm::vec4(1, -1, 1, 1),
            glm::vec4(1, 1, 1, 1), glm::vec4(-1, 1, 1, 1)
        };

        for (int i = 0; i < 8; ++i) {
            glm::vec4 worldPos = invVP * ndcCorners[i];
            corners[i] = glm::vec3(worldPos) / worldPos.w;
        }

        // Indices for drawing the frustum edges
        std::vector<GLuint> edgeIndices = {
            0, 1, 1, 2, 2, 3, 3, 0,    // Near face
            4, 5, 5, 6, 6, 7, 7, 4,    // Far face
            0, 4, 1, 5, 2, 6, 3, 7    // Connecting edges
        };

        // Activate the shader program
        glUseProgram(mShader.GetProgramID());

        // Set shader uniforms
        SetShaderUniforms(mShader.GetProgramID(), viewProj, color);

        // Ensure correct OpenGL state
        glDepthMask(GL_FALSE);   // No Depth-Writing
        glEnable(GL_BLEND);      // Enable Blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;

        if (vao == 0) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // Vertex attributes
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }

        // Bind the VAO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        ;

        GLsizei cornersDataSize = static_cast<GLsizei>(corners.size() * sizeof(glm::vec3));
        glBufferData(GL_ARRAY_BUFFER, cornersDataSize, corners.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        GLsizei edgeIndicesDataSize = static_cast<GLsizei>(edgeIndices.size() * sizeof(GLuint));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndicesDataSize, edgeIndices.data(), GL_DYNAMIC_DRAW);
        glDrawElements(GL_LINES, static_cast<GLsizei>(edgeIndices.size()), GL_UNSIGNED_INT, 0);


        // Unbind the VAO
        glBindVertexArray(0);

        // Deactivate the shader program
        glUseProgram(0);
    }

    void DebugRenderer::DrawPrimitiveWireframe(const glm::mat4& m2w, const Primitive* primitive, const glm::vec4& color) {
        if (!primitive) return;

        ActivateShader();

        // Set the transformation matrix
        glUniformMatrix4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_mvp"), 1, GL_FALSE, glm::value_ptr(m2w));

        // Set the color for wireframe drawing
        glUniform4fv(glGetUniformLocation(mShader.GetProgramID(), "uniform_color"), 1, glm::value_ptr(color));

        // Bind the VAO of the primitive
        primitive->Bind();

        // Draw the wireframe using GL_LINES
        primitive->Draw(GL_LINES);

        // Unbind the VAO
        Primitive::Unbind();

        DeactivateShader();
    }

} // namespace CS350

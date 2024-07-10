/**
 * @file DebugRenderer.cpp
 * @brief Defination of the Debug Renderer class
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 21/5/2024
 * @copyright Copyright (C) 2024 DigiPen Institute of Technology.
 */

#include "Primitive.hpp"

namespace CS350 {

    /**
     * @brief Constructor for the Primitive class.
     */
    Primitive::Primitive()
        : m_VAO(0), m_VBO(0), m_IBO(0), m_EBO(0),
        m_IndexCount(0), m_VertexCount(0), m_Shader(0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO); // Add this line
    }

    /**
     * @brief Destructor for the Primitive class.
     */
    Primitive::~Primitive() {
        glDeleteBuffers(1, &m_VBO);
        glDeleteVertexArrays(1, &m_VAO);
    }

    /**
     * @brief Binds the vertex array object (VAO).
     */
    void Primitive::Bind() const {
        glBindVertexArray(m_VAO);
    }

    /**
     * @brief Unbinds the vertex array object (VAO).
     */
    void Primitive::Unbind() {
        glBindVertexArray(0);
    }

    /**
     * @brief Draws the primitive.
     * @param mode The OpenGL primitive type to draw.
     */
    void Primitive::Draw(GLenum mode) const {
        Bind();
        glDrawArrays(mode, 0, m_VertexCount);
        Unbind();
    }

    /**
     * @brief Draws a part of the primitive.
     * @param mode The OpenGL primitive type to draw.
     * @param first The starting index of the vertex array.
     * @param count The number of vertices to draw.
     */
    void Primitive::DrawPart(GLenum mode, GLint first, GLsizei count) const {
        Bind();
        glDrawArrays(mode, first, count);
        Unbind();
    }

    /**
     * @brief Gets the vertex array object (VAO) ID.
     * @return The VAO ID.
     */
    GLuint Primitive::GetVAO() const {
        return m_VAO;
    }

    /**
     * @brief Gets the element buffer object (EBO) ID.
     * @return The EBO ID.
     */
    GLuint Primitive::GetEBO() const {
        return m_EBO;
    }

    /**
     * @brief Sets up the vertex buffer with provided vertices.
     * @param vertices The vector of vertices.
     */
    void Primitive::SetupBuffer(const std::vector<glm::vec3>& vertices) {
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        m_VertexCount = static_cast<GLsizei>(vertices.size());
    }

    /**
   * @brief Sets up the vertex buffer with provided vertices to draw a triangle.
   * @param v0 The first vertex of the triangle.
   * @param v1 The second vertex of the triangle.
   * @param v2 The third vertex of the triangle.
   */
    void Primitive::SetupTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
        std::vector<glm::vec3> vertices = { v0, v1, v2 };
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        m_VertexCount = static_cast<GLsizei>(vertices.size());

    }

    /**
   * @brief Sets up the vertex buffer and element buffer to draw a plane.
   * @param position The position of the plane.
   * @param normal The normal vector of the plane.
   * @param size The size of the plane.
   */
    void Primitive::SetupPlane(const glm::vec3& position, const glm::vec3& normal, float size) {
        // Determine two orthogonal vectors on the plane
        glm::vec3 tangent1;
        glm::vec3 tangent2;
        glm::vec3 up(0.0f, 1.0f, 0.0f); // Assuming the up direction is the positive y-axis
        if (glm::dot(normal, up) > 0.9f) {
            tangent1 = glm::vec3(1.0f, 0.0f, 0.0f); // If normal is close to up, use x-axis as tangent1
        }
        else {
            tangent1 = glm::normalize(glm::cross(normal, up)); // Otherwise, use the cross product
        }
        tangent2 = glm::normalize(glm::cross(normal, tangent1));

        // Compute vertices of the plane
        glm::vec3 v0 = position + tangent1 * size + tangent2 * size;
        glm::vec3 v1 = position - tangent1 * size + tangent2 * size;
        glm::vec3 v2 = position - tangent1 * size - tangent2 * size;
        glm::vec3 v3 = position + tangent1 * size - tangent2 * size;

        // Create vertex data for the plane
        std::vector<glm::vec3> planeVertices = { v0, v1, v2, v3 };

        // Create indices for drawing triangles
        std::vector<GLuint> planeIndices = {
            0, 1, 2, // First triangle
            0, 2, 3  // Second triangle
        };

        glBindVertexArray(m_VAO);

        // Setup vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        // glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(planeVertices.size() * sizeof(glm::vec3)), planeVertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(planeVertices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), planeVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Setup element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(planeVertices.size() * sizeof(glm::vec3)), planeVertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(planeIndices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), planeIndices.data(), GL_STATIC_DRAW);


        m_VertexCount = static_cast<GLsizei> (planeIndices.size());

        glBindVertexArray(0);
    }

    /**
    * @brief Generates vertices for a sphere.
    * @param xyDiscVertices Vector to store vertices on the XY-plane disc.
    * @param xzDiscVertices Vector to store vertices on the XZ-plane disc.
    * @param yzDiscVertices Vector to store vertices on the YZ-plane disc.
    * @param centerPosition The center position of the sphere.
    * @param radius The radius of the sphere.
    * @param discRadius The radius of the disc.
    * @param cameraPosition The position of the camera.
    */
    void Primitive::CreateSphereVertices(std::vector<glm::vec3>& xyDiscVertices,
        std::vector<glm::vec3>& xzDiscVertices,
        std::vector<glm::vec3>& yzDiscVertices,
        const glm::vec3& centerPosition, float discRadius) {

        // XY plane disc
        for (int i = 0; i <= 36; ++i) {
            float theta = static_cast<float>(i) * 2.0f * glm::pi<float>() / 36.0f;


            float x = glm::cos(theta);
            float y = glm::sin(theta);
            xyDiscVertices.push_back(glm::vec3(x, y, 0.0f) * discRadius + centerPosition);
        }

        // XZ plane disc
        for (int i = 0; i <= 36; ++i) {
            float theta = static_cast<float>(i) * 2.0f * glm::pi<float>() / 36.0f;

            float x = glm::cos(theta);
            float z = glm::sin(theta);
            xzDiscVertices.push_back(glm::vec3(x, 0.0f, z) * discRadius + centerPosition);
        }

        // YZ plane disc
        for (int i = 0; i <= 36; ++i) {
            float theta = static_cast<float>(i) * 2.0f * glm::pi<float>() / 36.0f;

            float y = glm::cos(theta);
            float z = glm::sin(theta);
            yzDiscVertices.push_back(glm::vec3(0.0f, y, z) * discRadius + centerPosition);
        }
    }

    /**
    * @brief Sets up the vertex buffer to draw a sphere.
    * @param centerPosition The center position of the sphere.
    * @param radius The radius of the sphere.
    * @param discRadius The radius of the disc.
    * @param cameraPosition The position of the camera.
    */
    void Primitive::SetupSphere(const glm::vec3& centerPosition, float radius, float discRadius, const glm::vec3& cameraPosition) {


        std::vector<glm::vec3> xyDiscVertices;
        std::vector<glm::vec3> xzDiscVertices;
        std::vector<glm::vec3> yzDiscVertices;
        std::vector<glm::vec3> horizontalDiscVertices;

        CreateSphereVertices(xyDiscVertices, xzDiscVertices, yzDiscVertices, centerPosition, discRadius);

        // Compute the camera-to-center vector
        glm::vec3 camToCenter = centerPosition - cameraPosition;

        // Compute the right and up vectors for the horizontal disc
        glm::vec3 right = glm::normalize(glm::cross(camToCenter, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, camToCenter));

        for (int i = 0; i <= 36; ++i) {
            float theta = static_cast<float>(i) * 2.0f * glm::pi<float>() / 36.0f;

            float x = glm::cos(theta);
            float y = glm::sin(theta);
            horizontalDiscVertices.push_back(centerPosition + radius * (x * right + y * up));
        }

        std::vector<glm::vec3> allVertices;
        allVertices.insert(allVertices.end(), xyDiscVertices.begin(), xyDiscVertices.end());
        allVertices.insert(allVertices.end(), xzDiscVertices.begin(), xzDiscVertices.end());
        allVertices.insert(allVertices.end(), yzDiscVertices.begin(), yzDiscVertices.end());
        allVertices.insert(allVertices.end(), horizontalDiscVertices.begin(), horizontalDiscVertices.end());

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        //  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(allVertices.size() * sizeof(glm::vec3)), allVertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(allVertices.size()) * static_cast<GLsizeiptr>(sizeof(glm::vec3)), allVertices.data(), GL_STATIC_DRAW);



        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Unbind the VAO
        m_VertexCount = static_cast<GLsizei>(allVertices.size());
    }





} // namespace CS350

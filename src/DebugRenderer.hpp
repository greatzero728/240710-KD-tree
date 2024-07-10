/**
 * @file DebugRenderer.hpp
 * @brief Declaration of the Debug Renderer class
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 21/5/2024
 * @copyright Copyright (C) 2024 DigiPen Institute of Technology.
 */

#ifndef DEBUG_RENDERER_HPP
#define DEBUG_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> 
#include "Shader.hpp"
#include "Primitive.hpp"


namespace CS350 {

    class DebugRenderer {
    public:
        DebugRenderer();
        DebugRenderer(const DebugRenderer& other);
        DebugRenderer& operator=(const DebugRenderer& other);


        ~DebugRenderer();

        void ActivateShader() const;
        static void DeactivateShader();

        // Debug drawing methods
        void DrawPointImmediate(const glm::mat4& viewProjection, const glm::vec3& position, const glm::vec4& color) const;
        void DrawPointImmediate(const glm::mat4& viewProjection, const glm::vec3& position, const glm::vec4& color, float size) const;
        void DrawSegmentImmediate(const glm::mat4& viewProjection, const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
        void DrawTriangleImmediate(const glm::mat4& viewProjection, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) const;
        void DrawAabbImmediate(const glm::mat4& vp, const glm::vec3& centre, const glm::vec3& size, const glm::vec4& color) const;
        void DrawAabbWireframeImmediate(const glm::mat4& viewProjection, const glm::vec3& center, const glm::vec3& size, const glm::vec4& color)const;
        void DrawPlaneImmediate(const glm::mat4& viewProjection, const glm::vec3& position, const glm::vec3& normal, float size, const glm::vec4& color) const;
        void DrawSphereImmediate(const glm::mat4& viewProjection, const glm::vec3& cameraPosition, const glm::vec3& centerPosition, float radius, const glm::vec4& color) const;
        void DrawFrustumImmediate(const glm::mat4& viewProj, const glm::mat4& frustumVP, const glm::vec4& color) const;
        void DrawFrustumWireframeImmediate(const glm::mat4& viewProj, const glm::mat4& frustumVP, const glm::vec4& color)const;
        void DrawPrimitiveWireframe(const glm::mat4& m2w, const Primitive* primitive, const glm::vec4& color);

       // glm::vec3 intersectPlanePlanePlane(const glm::vec4& plane1, const glm::vec4& plane2, const glm::vec4& plane3);
    private:
        Shader mShader;
        Primitive mPrimitive;

        static void CheckCompileErrors(GLuint shaderID, const std::string& type);
        static void CheckLinkErrors(GLuint programID);
    };

} // namespace CS350

#endif // DEBUG_RENDERER_HPP

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CS350 {

    class Camera {
    public:
        Camera();
        Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov_deg, float near, float far);

        void SetPosition(const glm::vec3& position);
        void SetTarget(const glm::vec3& target);
        void SetDirection(const glm::vec3& direction);
        void SetProjection(float fovY, const glm::ivec2& windowSize, float nearPlane, float farPlane);

        glm::vec3 position() const;
        glm::vec3 target() const;

        float fov_deg() const;
        float near() const;
        float far() const;

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 viewProj() const;

        void Update();

    private:
        glm::vec3 campos;
        glm::vec3 camtar;
        glm::vec3 camdir;
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 vp;
        glm::vec2 curspos;
        float fov;
        float nearPlane;
        float farPlane;
    };

} // namespace CS350

#endif // CAMERA_HPP

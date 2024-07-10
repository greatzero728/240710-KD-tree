#include "Camera.hpp"
#include "Utils.hpp"
#include "Math.hpp"

namespace CS350 {
    Camera::Camera() :
        campos(0.0f, 0.0f, 10.0f),
        camtar(0.0f, 0.0f, 0.0f),
        camdir(0.0f, 0.0f, -1.0f),
        view(1.0f),
        proj(1.0f),
        vp(1.0f),
        curspos(0.0f, 0.0f),
        fov(60.0f),
        nearPlane(0.01f),
        farPlane(2500.0f)
    {}

    void Camera::SetPosition(const glm::vec3& position) {
        campos = position;
    }

    void Camera::SetTarget(const glm::vec3& target) {
        camtar = target;
    }

    void Camera::SetDirection(const glm::vec3& direction) {
        camdir = direction;
    }

    void Camera::SetProjection(float fovY, const glm::ivec2& windowSize, float nearPlane, float farPlane) {
        float aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        proj = glm::perspective(glm::radians(fovY), aspectRatio, nearPlane, farPlane);
        fov = fovY;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;
    }

    void Camera::Update() {
        view = glm::lookAt(campos, camtar, glm::vec3(0.0, 1.0, 0.0));
        vp = proj * view;
    }

    glm::mat4 Camera::viewProj() const {
        return vp;
    }

    glm::vec3 Camera::position() const {
        return campos;
    }

    glm::vec3 Camera::target() const {
        return camtar;
    }

    float Camera::fov_deg() const {
        return fov;
    }

    float Camera::near() const {
        return nearPlane;
    }

    float Camera::far() const {
        return farPlane;
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return view;
    }

    glm::mat4 Camera::GetProjectionMatrix() const {
        return proj;
    }
}

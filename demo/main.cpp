#include "CS350Loader.hpp"
#include "DebugRenderer.hpp"
#include "DemoScene.hpp"
#include "KdTree.hpp"
#include "Primitive.hpp"
#include "Shader.hpp"
#include "Shapes.hpp"
#include "Window.hpp"
#include "ImGui.hpp"
#include <fstream>
#include <chrono>
#include <glm/gtx/color_space.hpp>
#include <stack>
#include <functional>
#include <sstream>
#include <vector>

namespace {
    constexpr vec2 c_window_size = vec2{ 1024, 576 } * 1.5f;

    void CameraMovementFly(CS350::Camera& camera, float dt, CS350::Window& window) {
        auto*       glfwWindow      = window.handle();
        auto        camera_dir      = camera.target() - camera.position();
        auto        camera_position = camera.position();
        static vec2 cursor_position{};

        // Cursor
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        glfwGetCursorPos(glfwWindow, &cursor_x, &cursor_y);
        { // Input
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_2)) {
                float speed = 10.0f;
                auto  side  = normalize(cross(camera_dir, { 0, 1, 0 }));

                if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT)) {
                    speed *= 4.0f;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_ALT)) {
                    speed /= 4.0f;
                }
                // Move
                if (glfwGetKey(glfwWindow, GLFW_KEY_W)) {
                    camera_position += normalize(camera_dir) * dt * speed;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_S)) {
                    camera_position -= normalize(camera_dir) * dt * speed;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_A)) {
                    camera_position -= normalize(side) * dt * speed;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_D)) {
                    camera_position += normalize(side) * dt * speed;
                }

                // View
                vec2 cursor_delta = { (float)cursor_x - cursor_position.x, (float)cursor_y - cursor_position.y };
                camera_dir        = vec3(vec4(camera_dir, 0) * rotate(glm::radians(15.0f) * 0.01f * cursor_delta.y, side));
                camera_dir        = vec3(vec4(camera_dir, 0) * rotate(glm::radians(15.0f) * 0.01f * cursor_delta.x, vec3(0, 1, 0)));
            }
            cursor_position    = { (float)cursor_x, (float)cursor_y };
            auto camera_target = camera_position + camera_dir;

            // Submit changes
            camera.SetPosition(camera_position);
            camera.SetTarget(camera_target);
            camera.SetProjection(camera.fov_deg(), window.size(), camera.near(), camera.far());
        }
    }
}

// void debug_draw_node(int n_index) {
//     auto& n = kd.nodes().at(n_index);
//
//     if (n.is_internal()) {
//         auto const& bv = kd.aabbs().at(n_index);
//         PipelineSettingsNode();
//
//         auto axis        = n.axis();
//         auto split_point = n.split();
//         // Set splitting plane for clipping
//         glm::vec3 plane_normal{};
//         glm::vec3 plane_point{};
//         plane_normal[axis] = 1;
//         plane_point        = kd.aabbs().at(n_index).GetCenter();
//         plane_point[axis]  = split_point;
//         CS350::Plane split_plane(plane_point, plane_normal);
//         float        size = bv.max[n.axis()] - bv.min[n.axis()];
//         debugger.DrawPlane(plane_point, plane_normal, size * 2.0f, c_color_split_plane);
//     }
// };

// void debug_traversal(float dt) {
//     if (!options.traversal_paused) {
//         // Update
//         float change     = 10.0f;
//         auto  update_pos = [&](vec2& pos, vec2& vel) {
//             vel[0] += glm::linearRand(-change, change) * dt;
//             vel[1] += glm::linearRand(-change, change) * dt;
//             vel[0] = glm::clamp(vel[0], -change, change);
//             vel[1] = glm::clamp(vel[1], -change, change);
//             pos += vel * dt;
//         };
//         update_pos(options.traversal_src, options.traversal_src_vel);
//         update_pos(options.traversal_dst, options.traversal_dst_vel);
//     }
//
//     // Actual test
//     auto spherical = [](vec2 angles, float r) {
//         angles      = glm::radians(angles);
//         float phi   = angles[0];
//         float theta = angles[1];
//         vec3  pos   = {};
//         pos.x       = glm::cos(phi) * glm::sin(theta);
//         pos.z       = glm::sin(phi) * glm::sin(theta);
//         pos.y       = glm::cos(theta);
//         return pos * r;
//     };
//     vec3 ray_start = spherical(options.traversal_src, options.traversal_radius);
//     vec3 ray_end   = spherical(options.traversal_dst, options.traversal_radius);
//
//     PipelineSettingsNormal();
//     debugger.DrawSphere(ray_start, 0.1f, { 1, 0, 0, 1 });
//     debugger.DrawSphere(ray_end, 0.1f, { 0, 0, 0, 1 });
//     debugger.DrawSegment(ray_start, ray_end, { 0.2, 0.2, 0.2, 1 });
//
//     CS350::Ray r(ray_start, ray_end - ray_start);
//     auto       inter = kd.get_closest(r, &options.kd_stats);
//     if (inter) {
//         auto pt = ray_start + (ray_end - ray_start) * inter.t;
//
//         glEnable(GL_DEPTH_TEST);
//         glDepthMask(GL_FALSE);
//
//         auto const& tri = triangles.at(inter.triangle_index);
//         glPointSize(2.0f);
//         debugger.DrawPoint(pt, { 1, 0, 0, 1 });
//         debugger.DrawSegment(ray_start, pt, { 1, 0, 0, 1 });
//
//         glDisable(GL_DEPTH_TEST);
//         debugger.DrawTriangle(tri[0], tri[1], tri[2], { 1, 1, 1, 1 });
//     }
// }


int main() {
    CS350::ChangeWorkdir("bin");
    CS350::Window::InitializeSystem();
    CS350::Window w({ 1920, 1080 });
    ImGuiInitialize(w.handle());
    { // Scene
        CS350::DemoScene scene;
        // Camera setup
        auto& cam = scene.camera();
        cam.SetPosition({ 0, 0, 10 });
        cam.SetProjection(60.0f, w.size(), 0.01f, 2500.0f);

        // mCamera.SetPosition({ -12.063, -52.3396, -25.3407 });
        // mCamera.SetTarget({ 0.479789, -3.10705, -3.16863 });

        cam.SetPosition({ -9.13937, -20.5272, -41.7185 });
        cam.SetTarget({ -9.608, 4.99564, 0.156429 });
        cam.SetProjection(50.0f, { 1920.0f, 1080.0f }, 0.01f, 1000.0f);

        while (!w.ShouldExit()) {
            // dt
            static auto lastTime = std::chrono::high_resolution_clock::now();
            float       dt       = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastTime).count();
            lastTime             = std::chrono::high_resolution_clock::now();

            // Window
            w.Update();

            // General rendering states
            ivec2 windowSize = w.size();
            glViewport(0, 0, windowSize.x, windowSize.y);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Camera
            CameraMovementFly(cam, dt, w);

            // Update matrices
            scene.Update();

            // Actual rendering
            scene.Render();
        }
    }

    CS350::Window::DestroySystem();
    return 0;
}


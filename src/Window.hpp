/**
 * @file Window.hpp
 * @brief Declaration of the Window class
 */

#pragma once

#include "OpenGl.hpp"
#include "Math.hpp"
#include <iostream>

namespace CS350 {

    class Window {
    public:
        Window(const glm::ivec2& size, GLFWwindow* windowHandle = nullptr);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        static void InitializeSystem();
        static void DestroySystem();

        void Update();

        bool ShouldExit() const;

        glm::ivec2 size() const;
        GLFWwindow* handle() const;

    private:
        GLFWwindow* m_WindowHandle;
        glm::ivec2  m_Size;
    };

} // namespace CS350

/**
 * @file Window.cpp
 * @brief Implementation of the Window class
 * @date 21/5/2024
 */

#include "Window.hpp"
#include <stdexcept>
#include <iostream> // Include for std::cerr and std::cout

namespace CS350 {

    void Window::InitializeSystem() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    void Window::DestroySystem() {
        glfwTerminate();
    }

    /**
     * @brief Construct a new Window object
     * @param size The size of the window
     * @param windowHandle The existing window handle (default is nullptr)
     */
    Window::Window(const glm::ivec2& size, GLFWwindow* windowHandle)
        : m_Size(size), m_WindowHandle(windowHandle) {

        if (!m_WindowHandle) {
            // Window and context setup
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

            m_WindowHandle = glfwCreateWindow(size.x, size.y, "CS350", nullptr, nullptr);
            if (m_WindowHandle == nullptr) {
                std::cerr << "GLFW unable to create OpenGL context - abort program\n";
                glfwTerminate();
                throw std::runtime_error("Could not create window");
            }
        }

        glfwMakeContextCurrent(m_WindowHandle);
        glfwSwapInterval(1); // Enable vsync

        // Initialize GLAD
        if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
            throw std::runtime_error("Failed to initialize GLAD");
        }
        if (GLAD_GL_VERSION_4_4 == 0) {
            throw std::runtime_error("Wrong OpenGL version");
        }

        // Set the viewport size
        glViewport(0, 0, size.x, size.y);

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        // glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        GLuint unusedFlags{ 0 };
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedFlags, GL_TRUE);

        // Debug: print to confirm window creation
        std::cout << "Window initialized with size: " << size.x << "x" << size.y << std::endl;
    }

    /**
     * @brief Destroy the Window object
     */
    Window::~Window() {
        // Destroy GLFW window if it was created by this object
        if (m_WindowHandle) {
            glfwDestroyWindow(m_WindowHandle);
            m_WindowHandle = nullptr;
        }
    }

    /**
     * @brief Update window events
     */
    void Window::Update() {
        glfwPollEvents();
        glfwSwapBuffers(m_WindowHandle);
        assert(glGetError() == GL_NO_ERROR);
    }

    /**
     * @brief Get the size of the window
     * @return The size of the window
     */
    glm::ivec2 Window::size() const {
        return m_Size;
    }

    /**
     * @brief Get the handle of the window
     * @return The handle of the window
     */
    GLFWwindow* Window::handle() const {
        return m_WindowHandle;
    }

    bool Window::ShouldExit() const {
        return glfwWindowShouldClose(m_WindowHandle);
    }

} // namespace CS350



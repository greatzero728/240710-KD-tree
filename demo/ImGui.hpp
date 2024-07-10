#ifndef __IMGUI_HPP__
#define __IMGUI_HPP__

#include "Math.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

// Guizmo
#include <ImGuizmo.h>
#include <stdexcept>

inline void ImGuiInitialize(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    if (!ImGui::CreateContext())
        throw std::runtime_error("Could not ImGui::CreateContext()");
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
        throw std::runtime_error("Could not ImGui_ImplGlfw_InitForOpenGL");
    if (!ImGui_ImplOpenGL3_Init("#version 150"))
        throw std::runtime_error("Could not ImGui_ImplOpenGL3_Init");
}

inline void ImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

inline void ImGuiEndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

namespace ImGui {
    inline void PushDisabled()
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    inline void PopDisabled()
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
    inline bool DragMatrix33(const char* name, glm::mat3* mat, bool readOnly = true)
    {
        bool changes = false;
        ImGui::PushID(name);
        ImGui::Text(name);
        if (readOnly) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        ImGui::PushItemWidth(-1);
        changes |= ImGui::DragFloat3("0", &(*mat)[0][0], 0.01f);
        changes |= ImGui::DragFloat3("1", &(*mat)[1][0], 0.01f);
        changes |= ImGui::DragFloat3("2", &(*mat)[2][0], 0.01f);
        ImGui::PopItemWidth();
        if (readOnly) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        ImGui::PopID();
        return changes;
    }

    bool Guizmo(vec3* position, mat4 const& v, mat4 const& p);
}

#endif // __IMGUI_HPP__

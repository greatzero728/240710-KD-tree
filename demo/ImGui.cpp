#include "ImGui.hpp"

bool ImGui::Guizmo(vec3* position, mat4 const& v, mat4 const& p) {
    // Make each guizmo different
    ImGuizmo::SetID(reinterpret_cast<unsigned long long>(reinterpret_cast<int const*>(position)));

    auto     m2w = glm::translate(*position);
    ImGuiIO& io  = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    if (ImGuizmo::Manipulate(&v[0][0], &p[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &m2w[0][0], NULL, NULL)) {
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(&m2w[0][0], matrixTranslation, matrixRotation, matrixScale);
        *position = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
        return true;
    }
    return false;
}

#include "DemoScene.hpp"
#include "CS350Loader.hpp"
#include "Primitive.hpp"
#include "Shapes.hpp"
#include "ImGui.hpp"
#include "Stats.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <glm/common.hpp>
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

namespace { // Asset related
    constexpr char const* cAssetPath = "assets/cs350/bunny-dense.cs350_binary";

    std::vector<CS350::Triangle> ToTriangles(CS350::CS350PrimitiveData const& primitive) {
        std::vector<CS350::Triangle> all_triangles;
        all_triangles.reserve(primitive.positions.size() / 3);
        assert(primitive.polygons.empty()); // Primitive should not be indexed
        for (size_t i = 0; i < primitive.positions.size(); i += 3) {
            all_triangles.push_back({ primitive.positions[i + 0],
                                      primitive.positions[i + 1],
                                      primitive.positions[i + 2] });
        }
        return all_triangles;
    }

    std::vector<CS350::Triangle> IndicesToTriangle(std::vector<CS350::Triangle> const& all_triangles, std ::vector<size_t> const& indices) {
        std::vector<CS350::Triangle> result;
        result.reserve(indices.size());
        for (auto idx : indices) {
            result.push_back(all_triangles.at(idx));
        }
        return result;
    }

    std::shared_ptr<CS350::Primitive> PrimitiveFromTriangles(std::vector<CS350::Triangle> const& triangles) {
        std::vector<vec3> positions;
        positions.reserve(triangles.size() * 3);
        for (auto const& t : triangles) {
            positions.push_back(t[0]);
            positions.push_back(t[1]);
            positions.push_back(t[2]);
        }
        return std::make_shared<CS350::Primitive>(CS350::Primitive(positions));
    }
}

namespace CS350 {

    DemoScene::DemoScene() {
        auto data            = CS350::LoadCS350Binary(cAssetPath);
        mTriangles           = ToTriangles(data);
        mPrimitive           = PrimitiveFromTriangles(mTriangles);
        mKdTreeCfg.max_depth = 50; // By default, no depth limit
        build_kdtree();
    }

    DemoScene::~DemoScene() {
    }

    void DemoScene::Update() {
        Stats::Instance().Reset();
        mCamera.Update();

        if (mOptions.traversal_test) {
            // Ray
            CS350::Ray r(mRayStart, mRayEnd - mRayStart);
            mKdTreeStats  = {};
            mIntersection = mKdTree.get_closest(mTriangles, r, &mKdTreeStats);
        }
    }

    void DemoScene::Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        PassDebug();

        ImGuiNewFrame();
        ImguiOptions();
        ImGuiEndFrame();
    }

    void DemoScene::PassDebug() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        mDebug.ActivateShader();

        { // Big shape
            mDebug.DrawPrimitiveWireframeImmediate(mCamera.viewProj(), mat4(1), mPrimitive.get(), { 0.5f, 0.5f, 0.5f, 1.0f });
        }

        { // Render leaves
            if (mOptions.draw_all_leaves) {
                for (size_t i = 0; i < mLeavesPrimitives.size(); ++i) {
                    mDebug.DrawPrimitiveImmediate(mCamera.viewProj(), mat4(1), mLeavesPrimitives[i].get(), mLeavesColors[i]);
                }
            }
        }

        // Selectede node debug
        if (!mCurrentNodePath.empty()) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            auto        n_index = mCurrentNodePath.back();
            auto const& node    = mKdTree.nodes().at(n_index);
            auto const& aabb    = mKdTree.aabbs().at(n_index);
            mDebug.DrawAabbWireframeImmediate(mCamera.viewProj(), aabb, { 1, 1, 1, 0.5f }); // Box

            // Splitting plane
            if (node.is_internal()) {
                vec3 splitPlanePosition         = aabb.GetCenter();
                splitPlanePosition[node.axis()] = node.split();
                vec3 normal                     = {};
                normal[node.axis()]             = 1;
                mDebug.DrawPlaneImmediate(mCamera.viewProj(), splitPlanePosition, normal, (aabb.max[node.axis()] - aabb.min[node.axis()]) * 1.1f, { 0.5, 0.5, 0.9, 0.75f });
            }
        }

        // Rays
        if (mOptions.traversal_test) {
            { // Traversed debug
                // Draw traversed nodes
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glDepthMask(GL_FALSE);
                glEnable(GL_DEPTH_TEST);

                if (mOptions.traversal_inspected >= int(mKdTreeStats.traversed_nodes.size()) || mOptions.show_all_traversed_nodes) {
                    for (size_t i = 0; i < mKdTreeStats.traversed_nodes.size(); ++i) {
                        auto const& aabb  = mKdTree.aabbs().at(mKdTreeStats.traversed_nodes.at(i));
                        vec4        color = glm::mix(vec4(1, 0, 0, 0.1f), vec4(0.2, 0.6, 1, 0.05), 1.0f - float(i) / float(mKdTreeStats.traversed_nodes.size()));
                        mDebug.DrawAabbImmediate(mCamera.viewProj(), aabb, color);
                    }
                } else {
                    auto        node_idx = mKdTreeStats.traversed_nodes.at(size_t(mOptions.traversal_inspected));
                    auto const& aabb     = mKdTree.aabbs().at(node_idx);
                    float       t        = 1.0f - float(mOptions.traversal_inspected) / float(mKdTreeStats.traversed_nodes.size());
                    vec4        color    = glm::mix(vec4(1, 0, 0, 0.85), vec4(0.2, 0.6, 1, 0.85), t);
                    glEnable(GL_BLEND);
                    mDebug.DrawAabbImmediate(mCamera.viewProj(), aabb, color); // Draw the box
                    glDisable(GL_BLEND);
                    mDebug.DrawAabbWireframeImmediate(mCamera.viewProj(), aabb, { 1, 1, 1, 1 }); // Draw box outline
                    glDisable(GL_CULL_FACE);
                    mDebug.DrawPrimitiveImmediate(mCamera.viewProj(), mat4(1), mNodesPrimitives[node_idx].get(), { 0.940, 0.583, 0.0470, 1.0f }); // Draw all triangles
                }

                glDisable(GL_BLEND);
                // Draw tested triangles
                for (size_t i = 0; i < mKdTreeStats.tested_triangles.size(); ++i) {
                    auto const& triangle = mTriangles.at(mKdTreeStats.tested_triangles.at(i));
                    mDebug.DrawTriangleImmediate(mCamera.viewProj(), triangle[0], triangle[1], triangle[2], vec4(0.0174, 0.870, 0.344, 1));
                }
            }

            { // RAY
                mDebug.DrawSphereImmediate(mCamera.viewProj(), mCamera.position(), mRayStart, 0.1f, { 1, 0, 0, 1 });
                mDebug.DrawSphereImmediate(mCamera.viewProj(), mCamera.position(), mRayEnd, 0.1f, { 0, 0, 0, 1 });
                mDebug.DrawSegmentImmediate(mCamera.viewProj(), mRayStart, mRayEnd, { 0.2, 0.2, 0.2, 1 });

                if (mIntersection) {
                    auto pt = mRayStart + (mRayEnd - mRayStart) * mIntersection.t;

                    auto const& tri = mTriangles.at(mIntersection.triangle_index);
                    mDebug.DrawSphereImmediate(mCamera.viewProj(), mCamera.position(), pt, 0.5f, { 1, 0, 1, 1 });
                    mDebug.DrawSegmentImmediate(mCamera.viewProj(), mRayStart, pt, { 1, 0, 0, 1 });
                    mDebug.DrawTriangleImmediate(mCamera.viewProj(), tri[0], tri[1], tri[2], { 1, 1, 1, 1 });
                }
            }
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mDebug.Render(mCamera);
        glDepthMask(GL_TRUE); // Restore
    }

    void DemoScene::ImguiOptions() {
        if (ImGui::Begin("KDTree")) {
            { // Change model
                if (ImGui::BeginCombo("File", "Select")) {
                    for (const auto& it : std::filesystem::directory_iterator("assets/cs350/")) {
                        if (it.is_directory()) {
                            continue;
                        }
                        if (it.path().extension().string().find("binary") != std::string::npos) {
                            if (ImGui::Selectable(it.path().string().c_str())) {
                                auto data  = CS350::LoadCS350Binary(it.path().string());
                                mTriangles = ToTriangles(data);
                                mPrimitive = PrimitiveFromTriangles(mTriangles);
                                build_kdtree();
                            }
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            // Options
            ImGui::Checkbox("Draw all leaves", &mOptions.draw_all_leaves);
            ImGui::DragFloat("cost_intersection", &mKdTreeCfg.cost_intersection, 0.01f, 0.0f, FLT_MAX);
            ImGui::DragFloat("cost_traversal", &mKdTreeCfg.cost_traversal, 0.01f, 0.0f, FLT_MAX);
            ImGui::DragInt("max_depth", &mKdTreeCfg.max_depth, 0.2f, 0, 9999);
            ImGui::DragInt("min_triangles", &mKdTreeCfg.min_triangles);
            if (ImGui::Button("Build kdtree")) {
                build_kdtree();
            }

            // Stats
            ImGui::PushDisabled();
            ImGui::Text("Original triangles: %lu", mTriangles.size());
            ImGui::Text("KdTree traversed nodes: %lu", mKdTreeStats.traversed_nodes.size());
            ImGui::Text("KdTree height: %lu", size_t(mKdTree.height()));
            ImGui::Text("Ray vs Aabb: %lu", size_t(CS350::Stats::Instance().rayVsAabb));
            ImGui::Text("Ray vs Triangle: %lu", size_t(CS350::Stats::Instance().rayVsTriangle));

            ImGui::PopDisabled();

            // Debug traversal
            ImGui::Separator();
            ImGui::Checkbox("Traversal test", &mOptions.traversal_test);

            ImGui::Checkbox("Show all nodes", &mOptions.show_all_traversed_nodes);
            if (mOptions.show_all_traversed_nodes) {
                mOptions.traversal_inspected = int(mKdTreeStats.traversed_nodes.size());
            } else {
                // Individual debug
                if (ImGui::Button("<")) {
                    mOptions.traversal_inspected--;
                    if (mOptions.traversal_inspected < 0) {
                        mOptions.traversal_inspected = 0;
                    }
                }
                ImGui::SameLine();
                ImGui::PushID("Slider");
                ImGui::SliderInt("", &mOptions.traversal_inspected, 0, int(mKdTreeStats.traversed_nodes.size()) - 1);
                ImGui::PopID();
                ImGui::SameLine();
                if (ImGui::Button(">")) {
                    mOptions.traversal_inspected++;
                    if (mOptions.traversal_inspected + 1 >= int(mKdTreeStats.traversed_nodes.size())) {
                        mOptions.traversal_inspected = int(mKdTreeStats.traversed_nodes.size()) - 1;
                    }
                }
            }

            ImGui::Guizmo(&mRayStart, mCamera.view(), mCamera.proj());
            ImGui::Guizmo(&mRayEnd, mCamera.view(), mCamera.proj());

            ImGui::Separator();

            // Traversal
            if (ImGui::Button("root")) {
                mCurrentNodePath = {};
                if (!mKdTree.empty()) {
                    mCurrentNodePath.push_back(0);
                }
            }

            if (!mCurrentNodePath.empty()) {
                auto        n_index = mCurrentNodePath.back();
                const auto& n       = mKdTree.nodes().at(n_index);

                if (n.is_internal()) {
                    if (ImGui::Button("children_left")) {
                        mCurrentNodePath.push_back(n_index + 1);
                    }
                    if (ImGui::IsItemHovered()) {
                        debug_draw_tris(int(n_index + 1));
                    }

                    if (ImGui::Button("children_right")) {
                        mCurrentNodePath.push_back(n.next_child());
                    }
                    if (ImGui::IsItemHovered()) {
                        debug_draw_tris(int(n.next_child()));
                    }
                } else {
                    // Disabled version
                    ImGui::PushDisabled();
                    if (ImGui::Button("children_left")) {
                    }
                    if (ImGui::Button("children_right")) {
                    }
                    ImGui::PopDisabled();
                }

                if (mCurrentNodePath.size() > 1) {
                    if (ImGui::Button("parent")) {
                        mCurrentNodePath.pop_back();
                    }
                } else {
                    // Disabled version
                    ImGui::PushDisabled();
                    if (ImGui::Button("parent")) {
                    }
                    ImGui::PopDisabled();
                }

                { // Info
                    static decltype(n_index) last_node = size_t(-1);
                    if (last_node != n_index) {
                        std::stringstream ss;
                        ss << "Node index: " << n_index << std::endl;
                        ss << "Node: " << &n << std::endl;
                        ss << "Node type: " << (n.is_internal() ? "INTERNAL" : "LEAF") << std::endl;
                        if (n.is_internal()) {
                            ss << "Split: " << n.split() << std::endl;
                            ss << "Axis: " << n.axis() << std::endl;
                            ss << "Height: " << mKdTree.height(int(n_index)) << std::endl;
                            ss << "\tLeft child idx: " << n_index + 1 << std::endl;
                            ss << "\tRight child idx: " << n.next_child() << std::endl;
                        } else {
                            ss << "Primitive start: " << n.primitive_start() << std::endl;
                            ss << "Primitive count: " << n.primitive_count() << std::endl;
                        }
                        mCurrentNodeInfo = ss.str();
                        last_node        = n_index;
                    }
                    ImGui::PushDisabled();
                    ImGui::SetNextItemWidth(-20.0f);
                    ImGui::InputTextMultiline("Info", &mCurrentNodeInfo, ImVec2(0, 200));
                    ImGui::PopDisabled();
                }
            }
        }
        ImGui::End();
    }

    void DemoScene::debug_draw_tris(int n_index) {
        mDebug.DrawAabbWireframe(mKdTree.aabbs().at(size_t(n_index)), { 1, 1, 1, 1 });
        mDebug.DrawPrimitive(mat4(1), mNodesPrimitives.at(size_t(n_index)).get(), { 1, 1, 1, 1 });
    }

    void DemoScene::build_kdtree() {
        mKdTree.build(mTriangles, mKdTreeCfg);
        mCurrentNodePath = {};

        // Clear nodes
        mLeavesPrimitives.clear();
        mLeavesColors.clear();
        mNodesPrimitives.clear();
        mNodesPrimitives.resize(mKdTree.nodes().size());

        // Create leaves
        std::function<void(int node_index)> generate_leaves;
        generate_leaves = [&](int node_index) {
            auto const& n                           = mKdTree.nodes().at(size_t(node_index));                              // Current node
            auto        tris_indices                = mKdTree.get_triangles(size_t(node_index));                           // Triangles of the current node
            auto        pri                         = PrimitiveFromTriangles(IndicesToTriangle(mTriangles, tris_indices)); // Primitive of the current node
            mNodesPrimitives.at(size_t(node_index)) = pri;                                                                 // Submit primitive

            if (n.is_internal()) {
                generate_leaves(node_index + 1);
                generate_leaves(int(n.next_child()));
            } else {
                mLeavesPrimitives.push_back(pri);

                // Random color
                vec3 hsv = {};
                hsv[0]   = glm::linearRand(0.0f, 360.0f);
                hsv[1]   = glm::linearRand(0.75f, 1.0f);
                hsv[2]   = glm::linearRand(0.75f, 1.0f);
                mLeavesColors.push_back(vec4(glm::rgbColor(hsv), 1));
            }
        };
        generate_leaves(0);
    }
}

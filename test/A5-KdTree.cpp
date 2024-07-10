#include "Common.hpp" // Utilities
#include "Geometry.hpp"
#include "KdTree.hpp"      // KDTree
#include "ShapeUtils.hpp"  // Intersection functions
#include "Shapes.hpp"      // Aabb/Triangles
#include "Utils.hpp"       // Others
#include "Stats.hpp"       // Stats
#include "PRNG.h"          // Random
#include "CS350Loader.hpp" // Loading assets
#include <chrono>
#include <gtest/gtest.h>
#include <ostream>
#include <vector>

namespace {

    struct Timer {
      private:
        using Clock             = std::chrono::high_resolution_clock;
        Clock::time_point start = Clock::now();

      public:
        void restart() { start = Clock::now(); }
        auto ellapsed_ms() const {
            auto diff = Clock::now() - start;
            return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        };
    };

    /**
     * @brief
     *  Converts a Primitive to a triangle array
     */
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

    struct KdTreeMesh {
        CS350::CS350PrimitiveData    data;
        std::vector<CS350::Triangle> triangles;
        vec3                         center;
        void                         Load(std::string const& path) {
            data      = CS350::LoadCS350Binary(path);
            triangles = ToTriangles(data);
            center    = {};
            for (auto const& tri : triangles) {
                for (int i = 0; i < 3; ++i) {
                    center += tri[i];
                }
            }
            center /= static_cast<float>(triangles.size());
        }
    };

    /**
     * @brief
     *  Dumps general information about a kdtree
     */
    void DumpStats(CS350::KdTree const& kdtree, std::ostream& os) {
        os << "\tHeight: " << kdtree.height() << std::endl;
        os << "\tNode count: " << kdtree.nodes().size() << std::endl;
    }

    /**
     * @brief
     *  Generates a random ray around an sphere, from the outside towards the center (approx)
     */
    CS350::Ray RandomRay(vec3 sphere_center, float internal_radius, float outer_radius) {
        vec3 ray_start = vec3(CS170::Utils::Random(internal_radius, outer_radius),
                              CS170::Utils::Random(internal_radius, outer_radius),
                              CS170::Utils::Random(internal_radius, outer_radius));
        vec3 ray_end   = vec3(CS170::Utils::Random(0.0f, internal_radius),
                            CS170::Utils::Random(0.0f, internal_radius),
                            CS170::Utils::Random(0.0f, internal_radius));
        vec3 ray_dir   = ray_end - ray_start;
        ray_start += sphere_center;
        return { ray_start, ray_dir };
    }

    /**
     * @brief
     *  Computes the closest intersection of a ray to all triangles
     */
    CS350::KdTree::Intersection ClosestIntersection(CS350::Ray const& ray, std::vector<CS350::Triangle> const& triangles) {
        CS350::KdTree::Intersection intersection{};
        intersection.t = -1.0f;
        for (size_t i = 0; i < triangles.size(); ++i) {
            auto const& triangle  = triangles[i];
            float       current_t = CS350::IntersectionTimeRayTriangle(ray, triangle);
            if (current_t >= 0.0f) {
                if (current_t < intersection.t || intersection.t < 0.0f) {
                    intersection.t              = current_t;
                    intersection.triangle_index = i;
                }
            }
        }
        return intersection;
    }

    struct PerformanceResults {
        float average_ray_vs_triangles;
        float average_ray_vs_aabb;
        float average_nodes_traversed;
        float average_duration_ms;
        float average_bf_duration_ms;
    };

    std::ostream& operator<<(std::ostream& os, PerformanceResults const& performance_results) {
        os << fmt::format("\tAverage ray vs triangle:           {}", performance_results.average_ray_vs_triangles) << std::endl;
        os << fmt::format("\tAverage ray vs aabb:               {}", performance_results.average_ray_vs_aabb) << std::endl;
        os << fmt::format("\tAverage nodes traversed:           {}", performance_results.average_nodes_traversed) << std::endl;
        os << fmt::format("\tAverage duration (kdtree):         {}ms", performance_results.average_duration_ms) << std::endl;
        os << fmt::format("\tAverage duration (brute force):    {}ms", performance_results.average_bf_duration_ms) << std::endl;
        return os;
    }

    /**
     * @brief
     *  Given a KdTree, tries different aspects and compares them to brute force
     */
    void TestKdTreePerformance(std::vector<CS350::Triangle> const& all_triangles,
                               CS350::KdTree const&                kdtree,
                               int                                 ray_count,
                               vec3                                sphere_center,
                               float                               internal_radius,
                               float                               outer_radius,
                               PerformanceResults&                 performance_results) {
        for (int i = 0; i < ray_count; ++i) {
            auto ray = RandomRay(sphere_center, internal_radius, outer_radius);

            // Brute force
            Timer                       timer;
            CS350::KdTree::Intersection intersection_bf{};
            CS350::Stats::Instance().Reset();
            intersection_bf = ClosestIntersection(ray, all_triangles);
            ASSERT_EQ(CS350::Stats::Instance().rayVsTriangle, all_triangles.size()) << "Stats not being kept updated";
            performance_results.average_bf_duration_ms += static_cast<float>(timer.ellapsed_ms());

            // KdTree
            timer.restart();
            CS350::KdTree::Intersection intersection_kdtree{};
            CS350::Stats::Instance().Reset();
            CS350::KdTree::DebugStats stats{};
            intersection_kdtree = kdtree.get_closest(all_triangles, ray, &stats);
            performance_results.average_ray_vs_triangles += static_cast<float>(CS350::Stats::Instance().rayVsTriangle);
            performance_results.average_ray_vs_aabb += static_cast<float>(CS350::Stats::Instance().rayVsAabb);
            performance_results.average_nodes_traversed += static_cast<float>(stats.traversed_nodes.size());
            performance_results.average_duration_ms += static_cast<float>(timer.ellapsed_ms());

            // The result of brute force and kdtree should be the same
            ASSERT_NEAR(intersection_bf.t, intersection_kdtree.t, 0.01f);
        }

        // Average results
        performance_results.average_nodes_traversed /= static_cast<float>(ray_count);
        performance_results.average_ray_vs_aabb /= static_cast<float>(ray_count);
        performance_results.average_ray_vs_triangles /= static_cast<float>(ray_count);
        performance_results.average_duration_ms /= static_cast<float>(ray_count);
        performance_results.average_bf_duration_ms /= static_cast<float>(ray_count);
    }

    KdTreeMesh g_dragon;
    KdTreeMesh g_bunny;
    KdTreeMesh g_bunny_dense;

    // Test setup
    class KdTree : public testing::Test {
      protected:
        static void SetUpTestSuite() {
            CS350::ChangeWorkdir();
            g_dragon.Load("./assets/cs350/dragon.cs350_binary");
            g_bunny.Load("./assets/cs350/bunny.cs350_binary");
            g_bunny_dense.Load("./assets/cs350/bunny-dense.cs350_binary");
        }

        void SetUp() override {
            CS350::ChangeWorkdir();
            CS350::Stats::Instance().Reset();
            ASSERT_EQ(CS350::Stats::Instance().frustumVsAabb, 0) << "Not reseting required stats";
            ASSERT_EQ(CS350::Stats::Instance().rayVsAabb, 0) << "Not reseting required stats";
        }
    };

    void EnsureAllTrianglesContained(KdTreeMesh const& mesh, CS350::KdTree const& kdTree) {
        for (size_t i = 0; i < kdTree.nodes().size(); ++i) {
            auto const& aabb        = kdTree.aabbs().at(i);
            auto const& tri_indices = kdTree.get_triangles(i);
            for (auto idx : tri_indices) {
                auto const& tri = mesh.triangles.at(idx);
                ASSERT_TRUE(CS350::ClassifyPointAabb(tri[0], aabb) <= 0); // Overlapping or inside
                ASSERT_TRUE(CS350::ClassifyPointAabb(tri[1], aabb) <= 0); // Overlapping or inside
                ASSERT_TRUE(CS350::ClassifyPointAabb(tri[2], aabb) <= 0); // Overlapping or inside
            }
        }
    }

    void EnsureDifferent(std::vector<size_t> const& lhs, std::vector<size_t> const& rhs) {
        // Ensure set like behavior (vectors are sorted)
        for (size_t i = 0; i + 1 < lhs.size(); ++i) {
            ASSERT_NE(lhs[i], lhs[i + 1]) << "Nodes should not contain duplicated triangles";
        }
        for (size_t i = 0; i + 1 < rhs.size(); ++i) {
            ASSERT_NE(rhs[i], rhs[i + 1]) << "Nodes should not contain duplicated triangles";
        }
    }

    bool IsLhsContainedInRhs(std::vector<size_t> const& lhs, std::vector<size_t> const& rhs) {
        for (auto lhs_element : lhs) {
            bool found_in_right = false;
            for (auto rhs_element : rhs) {
                if (lhs_element == rhs_element) {
                    found_in_right = true;
                    break;
                }
            }
            if (!found_in_right) {
                return true;
            }
        }
        return false;
    }

    void EnsureNodeSanity(CS350::KdTree const& kdTree) {
        for (size_t i = 0; i < kdTree.nodes().size(); ++i) {
            auto const& node               = kdTree.nodes().at(i);
            auto const& parent_tri_indices = kdTree.get_triangles(i);
            if (node.is_internal()) {
                auto left_tri_indices  = kdTree.get_triangles(i + 1);
                auto right_tri_indices = kdTree.get_triangles(node.next_child());
                std::sort(left_tri_indices.begin(), left_tri_indices.end());
                std::sort(right_tri_indices.begin(), right_tri_indices.end());
                ASSERT_LT(left_tri_indices.size(), parent_tri_indices.size()) << "Child nodes should have less triangles than parent\n"
                                                                              << "Problematic node: " << i;
                ASSERT_LT(right_tri_indices.size(), parent_tri_indices.size()) << "Child nodes should have less triangles than parent\n"
                                                                               << "Problematic node: " << i;
                ASSERT_FALSE(left_tri_indices.empty()) << "A child of an internal node should eventually contain triangles";
                ASSERT_FALSE(right_tri_indices.empty()) << "A child of an internal node should eventually contain triangles";
                EnsureDifferent(left_tri_indices, right_tri_indices);
                ASSERT_TRUE(IsLhsContainedInRhs(right_tri_indices, left_tri_indices)) << "Node " << i + 1
                                                                                      << " and " << node.next_child()
                                                                                      << " are not disjoint, they have the same triangles";
                ASSERT_TRUE(IsLhsContainedInRhs(left_tri_indices, right_tri_indices)) << "Node " << i + 1
                                                                                      << " and " << node.next_child()
                                                                                      << " are not disjoint, they have the same triangles";
            }
        }
    }
}

void BuildOnly(KdTreeMesh const& mesh, int max_depth) {
    CS350::KdTree         kdTree;
    CS350::KdTree::Config config;
    config.cost_intersection = 80;
    config.cost_traversal    = 1;
    config.max_depth         = max_depth;
    config.min_triangles     = 100;
    Timer timer;
    kdTree.build(mesh.triangles, config);
    auto duration = timer.ellapsed_ms();

    // Print debug
    auto debugFile = std::ofstream(fmt::format(".{}.dot", TestName()));
    kdTree.dump_graph(debugFile);
    std::cout << fmt::format("\tBuild duration: {}ms", duration) << std::endl;
    DumpStats(kdTree, std::cout);

    // Ensure validity
    EnsureAllTrianglesContained(mesh, kdTree);
    EnsureNodeSanity(kdTree);
}

void Efficiency(KdTreeMesh const& mesh, int max_depth) {
    CS350::KdTree         kdTree;
    CS350::KdTree::Config config;
    config.cost_intersection = 80;
    config.cost_traversal    = 1;
    config.max_depth         = max_depth;
    config.min_triangles     = 100;
    Timer timer;
    kdTree.build(mesh.triangles, config);
    auto duration = timer.ellapsed_ms();

    PerformanceResults performance_results{};
    TestKdTreePerformance(mesh.triangles, kdTree, 100, mesh.center, 5.0f, 100.0f, performance_results);
    auto debugFile = std::ofstream(fmt::format(".{}.dot", TestName()));
    kdTree.dump_graph(debugFile);
    DumpStats(kdTree, std::cout);
    std::cout << fmt::format("\tBuild duration: {}ms", duration) << std::endl;
    std::cout << performance_results << std::endl;

    ASSERT_LE(performance_results.average_duration_ms, performance_results.average_bf_duration_ms + 10.0f);

    // Special case test
    if (max_depth == 1) {
        ASSERT_EQ(kdTree.nodes().size(), 1);
        ASSERT_EQ(CS350::Stats::Instance().rayVsTriangle, mesh.triangles.size()) << "With a single node, all triangles should be tested";
    }
}

TEST_F(KdTree, BuildOnly_Bunny_1) { BuildOnly(g_bunny, 1); }
TEST_F(KdTree, BuildOnly_Bunny_2) { BuildOnly(g_bunny, 2); }
TEST_F(KdTree, BuildOnly_Bunny_4) { BuildOnly(g_bunny, 4); }
TEST_F(KdTree, BuildOnly_Bunny_8) { BuildOnly(g_bunny, 8); }
TEST_F(KdTree, BuildOnly_Bunny_Unlimited) { BuildOnly(g_bunny, 0); }
TEST_F(KdTree, BuildOnly_BunnyDense_1) { BuildOnly(g_bunny_dense, 1); }
TEST_F(KdTree, BuildOnly_BunnyDense_2) { BuildOnly(g_bunny_dense, 2); }
TEST_F(KdTree, BuildOnly_BunnyDense_4) { BuildOnly(g_bunny_dense, 4); }
TEST_F(KdTree, BuildOnly_BunnyDense_8) { BuildOnly(g_bunny_dense, 8); }
TEST_F(KdTree, BuildOnly_BunnyDense_16) { BuildOnly(g_bunny_dense, 16); }
TEST_F(KdTree, BuildOnly_BunnyDense_32) { BuildOnly(g_bunny_dense, 32); }
TEST_F(KdTree, BuildOnly_BunnyDense_Unlimited) { BuildOnly(g_bunny_dense, 0); }
TEST_F(KdTree, BuildOnly_Dragon_1) { BuildOnly(g_dragon, 1); }
TEST_F(KdTree, BuildOnly_Dragon_2) { BuildOnly(g_dragon, 2); }
TEST_F(KdTree, BuildOnly_Dragon_4) { BuildOnly(g_dragon, 4); }
TEST_F(KdTree, BuildOnly_Dragon_8) { BuildOnly(g_dragon, 8); }
TEST_F(KdTree, BuildOnly_Dragon_16) { BuildOnly(g_dragon, 16); }
TEST_F(KdTree, BuildOnly_Dragon_32) { BuildOnly(g_dragon, 32); }
TEST_F(KdTree, BuildOnly_Dragon_Unlimited) { BuildOnly(g_dragon, 0); }

TEST_F(KdTree, Efficiency_Bunny_1) { Efficiency(g_bunny, 1); }
TEST_F(KdTree, Efficiency_Bunny_2) { Efficiency(g_bunny, 2); }
TEST_F(KdTree, Efficiency_Bunny_4) { Efficiency(g_bunny, 4); }
TEST_F(KdTree, Efficiency_Bunny_8) { Efficiency(g_bunny, 8); }
TEST_F(KdTree, Efficiency_Bunny_Unlimited) { Efficiency(g_bunny, 0); }
TEST_F(KdTree, Efficiency_BunnyDense_1) { Efficiency(g_bunny_dense, 1); }
TEST_F(KdTree, Efficiency_BunnyDense_2) { Efficiency(g_bunny_dense, 2); }
TEST_F(KdTree, Efficiency_BunnyDense_4) { Efficiency(g_bunny_dense, 4); }
TEST_F(KdTree, Efficiency_BunnyDense_8) { Efficiency(g_bunny_dense, 8); }
TEST_F(KdTree, Efficiency_BunnyDense_16) { Efficiency(g_bunny_dense, 16); }
TEST_F(KdTree, Efficiency_BunnyDense_32) { Efficiency(g_bunny_dense, 32); }
TEST_F(KdTree, Efficiency_BunnyDense_Unlimited) { Efficiency(g_bunny_dense, 0); }
TEST_F(KdTree, Efficiency_Dragon_1) { Efficiency(g_dragon, 1); }
TEST_F(KdTree, Efficiency_Dragon_2) { Efficiency(g_dragon, 2); }
TEST_F(KdTree, Efficiency_Dragon_4) { Efficiency(g_dragon, 4); }
TEST_F(KdTree, Efficiency_Dragon_8) { Efficiency(g_dragon, 8); }
TEST_F(KdTree, Efficiency_Dragon_16) { Efficiency(g_dragon, 16); }
TEST_F(KdTree, Efficiency_Dragon_Unlimited) { Efficiency(g_dragon, 0); }
TEST_F(KdTree, Efficiency_Dragon_32) { Efficiency(g_dragon, 32); }

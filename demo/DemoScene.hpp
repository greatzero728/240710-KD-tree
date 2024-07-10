#ifndef DEMOSCENE_HPP
#define DEMOSCENE_HPP

#include "Camera.hpp"
#include "Primitive.hpp"
#include "Shapes.hpp"
#include "KdTree.hpp"
#include "DebugRenderer.hpp"
#include <memory>

// BVH usage
namespace CS350 {

    class DemoScene {
      private:
        Camera                            mCamera;
        DebugRenderer                     mDebug;
        std::vector<CS350::Triangle>      mTriangles;   // All the triangles of the current mesh
        std::shared_ptr<CS350::Primitive> mPrimitive;   // The actual mesh to draw (all triangles)
        CS350::KdTree::Config             mKdTreeCfg;   // KdTree configuration
        CS350::KdTree                     mKdTree;      //
        CS350::KdTree::DebugStats         mKdTreeStats; //

        std::vector<std::shared_ptr<CS350::Primitive>> mNodesPrimitives;  // Primitives representing the triangles of ALL CHILDREN RECURSIVELY
        std::vector<std::shared_ptr<CS350::Primitive>> mLeavesPrimitives; // Colored triangles
        std::vector<vec4>                              mLeavesColors;     // Colors of leaves

        std::vector<size_t> mCurrentNodePath; // Vector of node indices until currently inspected node
        std::string         mCurrentNodeInfo; // Information of selected node

        // RAY
        vec3                 mRayStart{};
        vec3                 mRayEnd{};
        KdTree::Intersection mIntersection; // Ray intersection result

        // OPTIONS
        struct {
            // Debug traversal
            bool traversal_test      = true;
            int  traversal_inspected = 0; // Currently inspected node, if ==max, disabled
            bool draw_all_leaves     = false;
            bool show_all_traversed_nodes = true; // If true, no slider
        } mOptions;

      public:
        DemoScene();
        DemoScene(const DemoScene&)            = delete;
        DemoScene(DemoScene&&)                 = delete;
        DemoScene& operator=(const DemoScene&) = delete;
        DemoScene& operator=(DemoScene&&)      = delete;
        ~DemoScene();
        void Update();
        void Render();

        Camera const& camera() const { return mCamera; }
        Camera&       camera() { return mCamera; }

        // Debug
        auto& debug() { return mDebug; }
        void  ImguiOptions();

      private:
        void PassDebug();
        void debug_draw_tris(int n_index);
        void build_kdtree();
    };
}

#endif // DEMOSCENE_HPP

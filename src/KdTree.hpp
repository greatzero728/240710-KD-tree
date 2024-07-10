#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <vector>
#include <iostream>
#include "Shapes.hpp"

namespace CS350 {
    /**
     * Basic KDTree
     */
    class KdTree {
      public:
        /**
         * Construction configuration
         */
        struct Config {
            float cost_traversal    = 1.0f;  // Part of heuristic equation
            float cost_intersection = 80.0f; // Part of heuristic equation
            int   max_depth         = 5;     // Should not create a tree bigger than this. 0 Means no limit
            int   min_triangles     = 50;    // If there are fewer than this triangles, should no split
        };

        /**
         * Statistics
         */
        struct DebugStats {
            std::vector<size_t> traversed_nodes;  // Node indices
            std::vector<size_t> tested_triangles; // Triangle indices
        };

        /**
         * Result of an intersection query
         */
        struct Intersection {
            // Index of the intersected triangle
            size_t triangle_index;

            // Time of intersection (t<0 if no intersection)
            float t;

            // Checks if intersected, for easy checks
            explicit operator bool() const { return t >= 0.0f; }
        };

        /**
         * KDTree Node structure
         */
        struct Node {
          private:

          public:
            void set_leaf(unsigned first_primitive_index, unsigned primitive_count);
            void set_internal(unsigned axis, float split_point, unsigned subnode_index);

            // Accessors
            [[nodiscard]] bool     is_leaf() const noexcept;
            [[nodiscard]] bool     is_internal() const noexcept;
            [[nodiscard]] unsigned primitive_count() const noexcept;
            [[nodiscard]] unsigned primitive_start() const noexcept;
            [[nodiscard]] unsigned next_child() const noexcept;
            [[nodiscard]] float    split() const noexcept;
            [[nodiscard]] unsigned axis() const noexcept;
        };

      private:
        std::vector<size_t> m_indices;              // All recorded triangles (may contain duplicates)
        std::vector<Node>   m_nodes;                // KDTree nodes
        std::vector<Aabb>   m_aabbs;                // AABBs of nodes (same order)
        Config              m_cfg;                  // Configuration
      public:
        void                       build(std::vector<Triangle> const& all_triangles, const Config& cfg);
        [[nodiscard]] Intersection get_closest(std::vector<Triangle> const& all_triangles, Ray r, DebugStats* stats) const;

        [[nodiscard]] const decltype(m_nodes)&   nodes() const noexcept { return m_nodes; }
        [[nodiscard]] const decltype(m_indices)& indices() const noexcept { return m_indices; }
        [[nodiscard]] const decltype(m_aabbs)&   aabbs() const noexcept { return m_aabbs; }
        [[nodiscard]] bool                       empty() const { return m_indices.empty(); }

        /**
         * Debug information
         */
        std::ostream&                     dump(std::ostream&) const;
        std::ostream&                     dump_graph(std::ostream&) const;
        [[nodiscard]] std::vector<size_t> get_triangles(size_t node_index) const; // Debug
        [[nodiscard]] int                 height() const;
        [[nodiscard]] int                 height(int node_idx) const;

      private:
        
    };
}
#endif // KDTREE_HPP

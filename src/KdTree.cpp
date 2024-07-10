#include <cassert>
#include <functional>
#include <glm/gtc/epsilon.hpp>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include "KdTree.hpp"
#include "Geometry.hpp"

namespace {
    float const cEpsilon = 0.001f;
}

namespace CS350 {


    /**
     *
     * @return std::ostream&
     */
    std::ostream& KdTree::dump(std::ostream& os) const {
        std::function<void(int n, int level)> node_dump;
        node_dump = [&](int n, int level) {
            Node const& node = m_nodes.at(size_t(n));

            std::string tab(size_t(level * 2), ' ');
            os << tab << "Node " << n << "[";
            if (node.is_internal()) {
                os << "internal, "
                   << "SplitPosition at " << char('x' + node.axis())
                   << "=" << node.split()
                   << "]\n"
                   << "Depth: " << level
                   << "Height: " << level
                   << "\n";
                node_dump(n + 1, level + 1);
                node_dump(node.next_child(), level + 1);
            } else {
                os << "leaf, "
                   << node.primitive_start() << ":" << node.primitive_start() + node.primitive_count()
                   << "]\n";
            }
        };
        node_dump(0, 0);

        return os;
    }

    /**
     * @brief
     * @return
     */
    std::ostream& KdTree::dump_graph(std::ostream& os) const {
        std::function<unsigned(int n)> node_triangle_count;
        node_triangle_count = [&](int n) {
            Node const& node = m_nodes.at(n);
            if (node.is_internal()) {
                return node_triangle_count(n + 1) + node_triangle_count(node.next_child());
            }
            return node.primitive_count();
        };

        std::function<void(int n, int n_parent)> node_dump;
        node_dump = [&](int n, int n_parent) {
            std::stringstream s;
            s << "NODE" << n;
            std::string name = s.str();
            os << "\t" << name << "[label=\"";
            Node const& node = m_nodes.at(size_t(n));
            if (node.is_internal()) {
                os << "SplitPosition: " << char('x' + node.axis()) << " at " << node.split() << "\\n"
                   << node_triangle_count(n) << " subtriangles";
            } else {
                os << node.primitive_count() << " triangles";
            }
            os << "\"];\n";

            // Edges
            if (n != 0) {
                s = std::stringstream();
                s << "NODE" << n_parent;
                std::string parent_name = s.str();
                os << "\t" << parent_name << " -> " << name << ";\n";
            }

            // Children
            if (node.is_internal()) {
                node_dump(n + 1, n);
                node_dump(node.next_child(), n);
            }
        };
        os << "digraph kdtree {\n";
        os << "\tnode[group=\"\", shape=none, style=\"rounded,filled\", fontcolor=\"#101010\"]\n";
        node_dump(0, 0);
        os << "}";
        return os;
    }
}

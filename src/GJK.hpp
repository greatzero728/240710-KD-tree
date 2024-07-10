#ifndef GJK_HPP
#define GJK_HPP

/**
 * @file GJK.hpp
 * @Author: Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 10/7/2024
 *
 * @brief Declaration of the Gilbert-Johnson-Keerthi (GJK) collision detection algorithm and supporting functions.
 * This file contains the declaration of the GJK algorithm used for collision detection between convex shapes.
 * It includes the definitions for the GjkSimplex class, support functions, and the main GJK function.
 * The GJK algorithm determines whether two convex shapes intersect by iteratively refining a simplex and checking
 * for the presence of the origin within it.
 *
 * @This code is part of a project for the DigiPen Institute of Technology.
 * @copyright Copyright (C) 2024 DigiPen Institute of Technology.
 */

#include "Math.hpp"

#include <vector>
#include <array>
#include <bitset>
#include <functional> // Debug purposes

namespace CS350 {

    struct GjkSimplex {
        std::array<vec3, 4> points = {}; // Simplex points
        std::bitset<4>      active = {}; // Determines which points are active

        // Adds a new point to the simplex
        void AddPoint(vec3 point);

        // Retrieves the new search direction and removes non-contributing points
        vec3 Iterate(vec3& closestPoint);

        // Keeps the active/points members without spaces in between, maintaining winding
        void Defragment();

        vec3 IterateSegment(vec3& closestPoint);
        vec3 IterateTriangle(vec3& closestPoint);
        vec3 IterateTetrahedron(vec3& closestPoint);
        void UpdatePoints(const std::initializer_list<vec3>& newPoints, size_t activeIndex);
        static vec3 ClosestPointOnSegment(const vec3& A, const vec3& AC, const vec3& AO);
        static vec3 TripleCrossProduct(const vec3& vec1, const vec3& vec2);
        vec3 HandleRegionOfABC(const vec3& ABC, const vec3& AO, vec3& closestPoint, const vec3& A, const vec3& B, const vec3& C);
    };

    // For debugging purposes
    using GjkSimplexDebugCallback = std::function<void(GjkSimplex const& simplex)>;

    // Tests if two convex shapes are colliding
    bool Gjk(std::vector<vec3> const& lhs,
        mat4 const& lhsM2w,
        std::vector<vec3> const& rhs,
        mat4 const& rhsM2w,
        const GjkSimplexDebugCallback& callback,
        size_t                         firstPointLhs = 0,
        size_t                         firstPointRhs = 0);

    bool OriginContained(const GjkSimplex& simplex);
    vec3 SupportFunction(std::vector<vec3> const& points, vec3 dir);
    vec3 TransformedSupportFunction(const std::vector<vec3>& shape, const mat4& transform, const vec3& direction);

}

#endif // GJK_HPP

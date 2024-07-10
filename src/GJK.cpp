/**
 * @file GJK.cpp
 * @Author: Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 10/7/2024
 * 
 * @brief Implementation of the Gilbert-Johnson-Keerthi (GJK) collision detection algorithm and supporting functions.
 * This file contains the implementation of the GJK algorithm used for collision detection between convex shapes.
 * It includes the definitions for the GjkSimplex class, support functions, and the main GJK function.
 * The GJK algorithm determines whether two convex shapes intersect by iteratively refining a simplex and checking
 * for the presence of the origin within it.
 * 
 * @This code is part of a project for the DigiPen Institute of Technology.
 * @copyright Copyright (C) 2024 DigiPen Institute of Technology.
 */


#include "GJK.hpp"
#include <iostream>
#include <Geometry.hpp>

const float Epsilon = 0.001f;

namespace CS350 {

    /**
     * @brief Adds a point to the simplex.
     *
     * @param point The point to add.
     */
    void GjkSimplex::AddPoint(vec3 point) {
        size_t i = 0;
        for (auto it = points.begin(); it != points.end(); ++it, ++i) {
            if (!active.test(i)) {
                *it = point;
                active.set(i);
                return;
            }
        }
    }

    /**
     * @brief Iterates the simplex to find the closest point on a segment.
     *
     * @param closestPoint The closest point on the segment.
     * @return The new direction vector.
     */
    vec3 GjkSimplex::IterateSegment(vec3& closestPoint) {
        vec3 A = points[0];
        vec3 B = points[1];
        vec3 AB = B - A; // Vector from A to B
        vec3 AO = -A; // Vector from A to the origin

        // Check if the origin is in the direction of AB
        if (glm::dot(AB, AO) > 0) {
            // Origin is in the region of AB
            closestPoint = glm::cross(glm::cross(AB, AO), AB); // Closest point on AB
            return closestPoint; // Return the new direction
        }

        // Origin is in the region of A
        points[0] = A; // Keep point A
        active[1] = false; // Deactivate point B
        closestPoint = AO; // Closest point is A
        return closestPoint; // Return the new direction
    }

    /**
     * @brief Iterates the simplex to find the closest point on a triangle.
     *
     * @param closestPoint The closest point on the triangle.
     * @return The new direction vector.
     */
    vec3 GjkSimplex::IterateTriangle(vec3& closestPoint) {
        vec3 A = points[2];
        vec3 B = points[1];
        vec3 C = points[0];
        vec3 AB = B - A; // Vector from A to B
        vec3 AC = C - A; // Vector from A to C
        vec3 AO = -A; // Vector from A to the origin
        vec3 ABC = glm::cross(AB, AC); // Normal of the ABC face

        // Check if the origin is in the region outside of AC
        vec3 ABCxAC = glm::cross(ABC, AC);
        if (glm::dot(ABCxAC, AO) > 0) {
            if (glm::dot(AC, AO) > 0) {
                // Origin is in the region of AC
                UpdatePoints({ C, A }, 2); // Update points to the AC edge
                closestPoint = ClosestPointOnSegment(A, AC, AO); // Find the closest point on AC
                return TripleCrossProduct(AC, AO); // Return the new direction
            }
            // Origin is in the region of AB or the origin
            UpdatePoints({ B, A }, 2); // Update points to the AB edge
            return IterateSegment(closestPoint); // Recurse on the AB edge
        }

        // Check if the origin is in the region outside of AB
        vec3 ABxABC = glm::cross(AB, ABC);
        if (glm::dot(ABxABC, AO) > 0) {
            // Origin is in the region of AB
            UpdatePoints({ B, A }, 2); // Update points to the AB edge
            return IterateSegment(closestPoint); // Recurse on the AB edge
        }

        // Origin is in the region of ABC
        return HandleRegionOfABC(ABC, AO, closestPoint, A, B, C); // Handle the region of ABC
    }


    /**
     * @brief Updates the points in the simplex.
     *
     * @param newPoints The new points to update.
     * @param activeIndex The index of the active point.
     */
    void GjkSimplex::UpdatePoints(const std::initializer_list<vec3>& newPoints, size_t activeIndex) {
        const auto* pointIt = newPoints.begin();
        std::array<vec3, 4>::iterator pointDestIt = points.begin();
        while (pointIt != newPoints.end() && pointDestIt != points.end()) {
            *pointDestIt++ = *pointIt++;
        }
        active[activeIndex] = false;
    }



    /**
     * @brief Finds the closest point on a segment.
     *
     * @param A The first point of the segment.
     * @param AC The vector from A to C.
     * @param AO The vector from A to the origin.
     * @return The closest point on the segment.
     */
    vec3 GjkSimplex::ClosestPointOnSegment(const vec3& A, const vec3& AC, const vec3& AO) {
        return A + glm::dot(AO, AC) / glm::dot(AC, AC) * AC;
    }


    /**
     * @brief Calculates the triple cross product of two vectors.
     *
     * @param vec1 The first vector.
     * @param vec2 The second vector.
     * @return The triple cross product of the vectors.
     */
    vec3 GjkSimplex::TripleCrossProduct(const vec3& vec1, const vec3& vec2) {
        return glm::cross(glm::cross(vec1, vec2), vec1);
    }

    /**
     * @brief Handles the region of the simplex when the origin is within the triangle.
     *
     * @param ABC The normal vector of the triangle.
     * @param AO The vector from A to the origin.
     * @param closestPoint The closest point on the triangle.
     * @param A The first point of the triangle.
     * @param B The second point of the triangle.
     * @param C The third point of the triangle.
     * @return The new direction vector.
     */
    vec3 GjkSimplex::HandleRegionOfABC(const vec3& ABC, const vec3& AO, vec3& closestPoint, const vec3& A, const vec3& B, const vec3& C) {
        float a = glm::dot(ABC, AO);
        if (a > 0) {
            UpdatePoints({ C, B, A }, 3);
            closestPoint = -glm::dot(ABC, AO) * ABC;
            return ABC;
        }
        UpdatePoints({ B, C, A }, 3);
        closestPoint = -glm::dot(ABC, AO) * ABC;
        return -ABC;
    }


    /**
     * @brief Iterates the simplex to find the closest point on a tetrahedron.
     *
     * @param closestPoint The closest point on the tetrahedron.
     * @return The new direction vector.
     */
    vec3 GjkSimplex::IterateTetrahedron(vec3& closestPoint) {
        vec3 A = points[0];
        vec3 B = points[1];
        vec3 C = points[2];
        vec3 D = points[3];
        vec3 AB = B - A; // Vector from A to B
        vec3 AC = C - A; // Vector from A to C
        vec3 AD = D - A; // Vector from A to D
        vec3 AO = -A; // Vector from A to the origin

        vec3 ABC = glm::cross(AB, AC); // Normal of the ABC face
        vec3 ACD = glm::cross(AC, AD); // Normal of the ACD face
        vec3 ADB = glm::cross(AD, AB); // Normal of the ADB face

        // Check if the origin is in the region of ABC
        if (glm::dot(ABC, AO) > 0) {
            UpdatePoints({ A, B, C }, 3); // Update points to the ABC face
            return IterateTriangle(closestPoint); // Recurse on the ABC face
        }

        // Check if the origin is in the region of ACD
        if (glm::dot(ACD, AO) > 0) {
            UpdatePoints({ A, C, D }, 3); // Update points to the ACD face
            return IterateTriangle(closestPoint); // Recurse on the ACD face
        }

        // Check if the origin is in the region of ADB
        if (glm::dot(ADB, AO) > 0) {
            UpdatePoints({ A, D, B }, 3); // Update points to the ADB face
            return IterateTriangle(closestPoint); // Recurse on the ADB face
        }

        // Origin is inside the tetrahedron
        closestPoint = vec3(0, 0, 0); // Origin is the closest point
        return closestPoint; // Return zero vector indicating the origin is inside
    }


    /**
     * @brief Checks if the origin is contained within the simplex.
     *
     * @param simplex The simplex to check.
     * @return True if the origin is contained within the simplex, false otherwise.
     */
    bool OriginContained(const GjkSimplex& simplex) {
        vec3 origin{ 0 };
        switch (simplex.active.count()) {
        case 1:
            return ClassifyPointPoint(origin, simplex.points[0]);
        case 2:
            return ClassifyPointSegment(origin, simplex.points[0], simplex.points[1]);
        case 3:
            return ClassifyPointTriangle(origin, simplex.points[0], simplex.points[1], simplex.points[2]);
        default: {
            vec3 p0 = simplex.points[0];
            vec3 p1 = simplex.points[1];
            vec3 p2 = simplex.points[2];
            vec3 p3 = simplex.points[3];
            return ClassifyPointTetrahedron(origin, p0, p1, p2, p3);
        }
        }
    }


    /**
     * @brief Finds the support point in a given direction.
     *
     * @param points The points of the shape.
     * @param dir The direction vector.
     * @return The support point in the given direction.
     */
    vec3 SupportFunction(const std::vector<vec3>& points, vec3 dir) {
        vec3 extreme = points[0];
        float extreme_dot = glm::dot(extreme, dir);
        for (size_t i = 1; i < points.size(); ++i) {
            float point_dot = glm::dot(points[i], dir);
            if (point_dot > extreme_dot) {
                extreme = points[i];
                extreme_dot = point_dot;
            }
        }
        return extreme;
    }


    /**
     * @brief Finds the support point in a transformed shape.
     *
     * @param shape The points of the shape.
     * @param transform The transformation matrix.
     * @param direction The direction vector.
     * @return The support point in the transformed shape.
     */
    vec3 TransformedSupportFunction(const std::vector<vec3>& shape, const mat4& transform, const vec3& direction) {
        vec3 localDir = vec3(glm::inverse(transform) * vec4(direction, 0.0f));
        vec3 localSupport = SupportFunction(shape, localDir);
        return vec3(transform * vec4(localSupport, 1.0f));
    }


    /**
     * @brief Defragments the simplex by removing inactive points.
     */
    void GjkSimplex::Defragment() {
        size_t j = 0;
        for (size_t i = 0; i < points.size(); ++i) {
            if (active.test(i)) {
                if (i != j) {
                    points.at(j) = points.at(i); // Use at() for bounds checking
                    active.reset(i);
                    active.set(j);
                }
                j++;
            }
        }
    }



    /**
     * @brief Iterates the simplex to find the closest point.
     *
     * @param closestPoint The closest point found.
     * @return The new direction vector.
     */
    vec3 GjkSimplex::Iterate(vec3& closestPoint) {
        int count = static_cast<int>(active.count());

        switch (count) {
        case 1:
            closestPoint = points[0];
            return -closestPoint;

        case 2:
            return IterateSegment(closestPoint);

        case 3:
            return IterateTriangle(closestPoint);

        case 4:
            return IterateTetrahedron(closestPoint);

        default:
            // Handle unexpected values of active.count()
            std::cerr << "Error: Invalid count value " << count << " in GjkSimplex::Iterate" << std::endl;
            // Returning a default value or handling the error appropriately
            return vec3(0.0f, 0.0f, 0.0f);
        }
    }


    /**
     * @brief Implements the GJK algorithm to check for collision.
     *
     * @param lhs The points of the first shape.
     * @param lhsM2w The model-to-world transformation matrix for the first shape.
     * @param rhs The points of the second shape.
     * @param rhsM2w The model-to-world transformation matrix for the second shape.
     * @param callback The callback function for debugging.
     * @param firstPointLhs The index of the first point in the first shape.
     * @param firstPointRhs The index of the first point in the second shape.
     * @return True if the shapes collide, false otherwise.
     */
    bool Gjk(const std::vector<vec3>& lhs, const mat4& lhsM2w, const std::vector<vec3>& rhs, const mat4& rhsM2w,
        const GjkSimplexDebugCallback& callback, size_t firstPointLhs, size_t firstPointRhs) {

        GjkSimplex simplex; // Initialize the simplex used for the GJK algorithm
        vec3 direction = { 1, 0, 0 }; // Initial search direction

        // Get the initial support points from both shapes
        vec3 firstSupportLhs = lhs[firstPointLhs];
        vec3 firstSupportRhs = rhs[firstPointRhs];

        // Calculate the initial support point for the Minkowski difference
        vec3 firstSupport = TransformedSupportFunction(lhs, lhsM2w, firstSupportLhs) -
            TransformedSupportFunction(rhs, rhsM2w, -firstSupportRhs);
        simplex.AddPoint(firstSupport); // Add the initial support point to the simplex
        direction = -firstSupport; // Set the initial direction to the negative of the support point

        // Main loop of the GJK algorithm
        while (!OriginContained(simplex)) { // Continue until the origin is contained in the simplex
            if (callback) {
                callback(simplex); // Call the debug callback if provided
            }

            // Get a new support point in the current direction
            vec3 support = TransformedSupportFunction(lhs, lhsM2w, direction) -
                TransformedSupportFunction(rhs, rhsM2w, -direction);
            if (glm::dot(support, direction) <= 0) { // Check if the support point does not advance in the direction
                return false; // No collision
            }

            simplex.AddPoint(support); // Add the new support point to the simplex
            if (callback) {
                callback(simplex); // Call the debug callback if provided
            }

            simplex.Defragment(); // Defragment the simplex by removing redundant points
            vec3 closestPoint;
            direction = simplex.Iterate(closestPoint); // Iterate the simplex and get the new search direction

            float currentDistance = glm::length2(closestPoint); // Calculate the squared distance to the origin
            if (currentDistance < Epsilon * Epsilon) { // Check if the distance is within the tolerance
                return true; // Collision detected
            }
        }

        if (callback) {
            callback(simplex); // Call the debug callback if provided
        }
        return true; // Collision detected
    }


}

/**
 * @file ShapeUtils.cpp
 * @brief Defination of utility functions for shape creation
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 6/6/2024
 * @copyright copyright (C) 2024 DigiPen Institute of Technology.
 */



#include <random>
#include <algorithm>
#include "Geometry.hpp"
#include "ShapeUtils.hpp"

namespace CS350
{

     /**
     * @brief Creates an axis-aligned bounding box (AABB) using brute force method.
     *
     * @param positions Array of positions.
     * @param size Number of positions.
     * @param transform Transformation matrix.
     * @return Aabb Axis-aligned bounding box.
     */
    Aabb CreateAabbBruteForce(const vec3* positions, size_t size, const glm::mat4x4& transform)
    {
        // take first point as reference
        vec3 min;
        vec3 max;

        min = max = vec3{ transform * vec4{positions[0], 1.f} };

        // loop through the rest of the points searching for min and max
        for (unsigned i = 1; i < size; ++i)
        {
            vec3 pos = vec3{ transform * vec4{positions[i], 1.f} };
            if (min.x > pos.x) { min.x = pos.x; }
            if (max.x < pos.x) { max.x = pos.x; }
            if (min.y > pos.y) { min.y = pos.y; }
            if (max.y < pos.y) { max.y = pos.y; }
            if (min.z > pos.z) { min.z = pos.z; }
            if (max.z < pos.z) { max.z = pos.z; }
        }
        return Aabb{ min, max };
    }

    /**
     * @brief Creates an axis-aligned bounding box (AABB) using the furthest point method.
     *
     * @param positions Array of positions.
     * @param size Number of positions.
     * @param transform Transformation matrix.
     * @return Aabb Axis-aligned bounding box.
     */
    Aabb CreateAabbFurthestPoint(const glm::vec3* positions, size_t size, const glm::mat4x4& transform)
    {
        vec3 center = vec3(transform[3]);
        float maxDistanceSquared = 0.0f; // Square of the maximum distance

        // Calculate the maximum distance squared from the center to any point
        for (unsigned i = 0; i < size; ++i)
        {
            float distanceSquared = glm::distance2(center, vec3{ transform * vec4{positions[i], 1.f} });
            maxDistanceSquared = std::max(maxDistanceSquared, distanceSquared);
        }

        // Compute the maximum distance (by taking the square root of maxDistanceSquared)
        float maxDistance = std::sqrt(maxDistanceSquared);

        // Return the AABB
        return Aabb{ center - maxDistance, center + maxDistance };
    }

     /**
     * @brief Creates a bounding sphere using the centroid method.
     *
     * @param positions Array of positions.
     * @param size Number of positions.
     * @param transform Transformation matrix.
     * @return Sphere Bounding sphere.
     */
    Sphere CreateSphereFurthestPoint(const glm::vec3* positions, size_t size, const glm::mat4x4& transform)
    {
        vec3 center = vec3(transform[3]);
        float distance = glm::distance(center, vec3{ transform * vec4{positions[0], 1.f} });
        for (unsigned i = 1; i < size; ++i)
        {
            float points_d = glm::distance(center, vec3{ transform * vec4{positions[i], 1.f} });
            if (points_d > distance)
            {
                distance = points_d;
            }
        }
        return Sphere{ center, distance };
    }


     /**
     * @brief Creates a bounding sphere using the centroid method.
     *
     * @param positions Array of positions.
     * @param size Number of positions.
     * @param transform Transformation matrix.
     * @return Sphere Bounding sphere.
     */
    Sphere CreateSphereCentroid(const vec3* positions, size_t size, const glm::mat4x4& transform)
    {
        // compute centroid as average point
        Sphere sphere;
        for (unsigned i = 0; i < size; ++i)
        {
            sphere.center += vec3{ transform * vec4{positions[i], 1.f} };
        }
        sphere.center /= size;

        // find most extreme point from centroid
        for (unsigned i = 0; i < size; ++i)
        {
            float length = glm::length(sphere.center - vec3{ transform * vec4{ positions[i], 1.f } });
            if (length > sphere.radius)
            {
                sphere.radius = length;
            }
        }
        return sphere;
    }


    /**
     * @brief Creates a bounding sphere using Ritter's method.
     *
     * @param positions Array of positions.
     * @param size Number of positions.
     * @param transform Transformation matrix.
     * @return Sphere Bounding sphere.
     */
    Sphere CreateSphereRitter(const vec3* positions, size_t size, const glm::mat4x4& transform)
    {
        // get the distance between every axis of the first points as reference
        vec3 pos0 = vec3(transform * vec4(positions[0], 1.f));
        vec3 pos1 = vec3(transform * vec4(positions[1], 1.f));
        vec3 X1 = pos0;
        vec3 X2 = pos1;
        float sepX = glm::length(pos0.x - pos1.x);
        vec3 Y1 = pos0;
        vec3 Y2 = pos1;
        float sepY = glm::length(pos0.y - pos1.y);
        vec3 Z1 = pos0;
        vec3 Z2 = pos1;
        float sepZ = glm::length(pos0.z - pos1.z);

        // check the rest of the points finding the greatest distance
        for (unsigned i = 1; i < size - 1; ++i)
        {
            pos0 = vec3(transform * vec4(positions[i], 1.f));
            pos1 = vec3(transform * vec4(positions[i + 1], 1.f));

            float new_sepX = glm::length(pos0.x - pos1.x);
            if (new_sepX > sepX)
            {
                X1 = pos0; X2 = pos1;
                sepX = new_sepX;
            }

            float new_sepY = glm::length(pos0.y - pos1.y);
            if (new_sepY > sepY)
            {
                Y1 = pos0; Y2 = pos1;
                sepY = new_sepY;
            }

            float new_sepZ = glm::length(pos0.z - pos1.z);
            if (new_sepZ > sepZ)
            {
                Z1 = pos0; Z2 = pos1;
                sepZ = new_sepZ;
            }
        }

        // take biggest distance and use it as center and radius
        vec3 P1;
        vec3 P2;
        float diameter = 0.0f;
        if (sepX > sepY && sepX > sepZ)
        {
            P1 = X1; P2 = X2; diameter = sepX;
        }
        else if (sepY > sepZ)
        {
            P1 = Y1; P2 = Y2; diameter = sepY;
        }
        else
        {
            P1 = Z1; P2 = Z2; diameter = sepZ;
        }

        Sphere sphere((P1 + P2) / 2.f, diameter / 2.f);

        // check that every point fits the sphere and expand otherwise
        for (unsigned i = 0; i < size; ++i)
        {
            vec3 pos = vec3(transform * vec4(positions[i], 1.f));
            if (!OverlapPointSphere(pos, sphere.center, sphere.radius))
            {
                // compute back of the sphere
                vec3 back = sphere.center - sphere.radius * (pos - sphere.center) / glm::length(pos - sphere.center);
                // expand center in the direction of positions[i]
                sphere.center = (pos + back) / 2.f;
                // expand radius
                sphere.radius = glm::length(pos - back) / 2.f;
            }
        }

        return sphere;
    }


    /**
     * @brief Creates a bounding sphere using an iterative approach.
     *
     * @param positions Array of positions.
     * @param size Number of positions.
     * @param iterations Number of iterations.
     * @param shrinkRatio Shrink ratio.
     * @param transform Transformation matrix.
     * @return Sphere Bounding sphere.
     */
    Sphere CreateSphereIterative(const vec3* positions, size_t size,  int iterations,  float shrinkRatio, const glm::mat4x4& transform)
    {
        // get sphere with ritters method
        Sphere original_sphere = CreateSphereRitter(positions, size, transform);
        Sphere sphere(original_sphere.center, original_sphere.radius);

        // vector to shuffle positions
        std::vector<vec3> shuffled;
        shuffled.reserve(size);
        for (size_t i = 0; i < size; ++i)
        {
            shuffled.push_back(positions[i]);
        }

        // iterate the given times
        for (int it = 0; it < iterations; ++it)
        {
            sphere.center = original_sphere.center;
            // go shrinking the sphere in each iteration
            sphere.radius = original_sphere.radius - shrinkRatio;

            // shuffle the vector to iterate randomly
            std::shuffle(shuffled.begin(), shuffled.end(), std::default_random_engine{});

            for (auto position : shuffled)
            {
                vec3 pos = vec3(transform * vec4(position, 1.f));
                if (!OverlapPointSphere(pos, sphere.center, sphere.radius))
                {
                    // compute back of the sphere
                    vec3 back = sphere.center - sphere.radius * (pos - sphere.center) / glm::length(pos - sphere.center);
                    // expand center in the direction of positions[i]
                    sphere.center = (pos + back) / 2.f;
                    // expand radius
                    sphere.radius = glm::length(pos - back) / 2.f;
                }
            }

            // we got a better sphere than the one we had
            if (sphere.radius < original_sphere.radius)
            {
                original_sphere.center = sphere.center;
                original_sphere.radius = sphere.radius;
            }
        }
        return original_sphere;
    }

}

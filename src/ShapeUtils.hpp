/**
 * @file ShapeUtils.hpp
 * @brief Declaration of utility functions for shape creation
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 6/6/2024
 * @copyright copyright (C) 2024 DigiPen Institute of Technology.
 */


#ifndef __SHAPEUTILS_HPP__
#define __SHAPEUTILS_HPP__

#include <vector>
#include "Shapes.hpp"

namespace CS350
{
    Aabb CreateAabbBruteForce(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    Aabb CreateAabbFurthestPoint(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    Sphere CreateSphereFurthestPoint(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    Sphere CreateSphereCentroid(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    Sphere CreateSphereRitter(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    Sphere CreateSphereIterative(const glm::vec3* positions, size_t size, int iterations, float shrinkRatio, const glm::mat4x4& transform = glm::mat4x4{ 1.f });

}

#endif // __SHAPEUTILS_HPP__

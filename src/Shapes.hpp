/**
* @file Shapes.hpp
* @brief Implementation of the Aabb and Sphere classes
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 6/6/2024
 * @copyright copyright (C) 2024 DigiPen Institute of Technology.
 */


#ifndef __SHAPES_HPP__
#define __SHAPES_HPP__

#include "Math.hpp"
#include <array> 
#include "Stats.hpp"
#include"Geometry.hpp"

namespace CS350 {

    struct Aabb {
        Aabb();
        Aabb(glm::vec3 min, glm::vec3 max);
        bool operator==(const Aabb& other) const {
            return min == other.min && max == other.max;
        }

        bool operator!=(const Aabb& other) const {
            return !(*this == other);
        }
        Aabb Transform(const glm::mat4x4& transform);
        float SurfaceArea() const;
        float Volume() const;
        Aabb GetBoundingBox() const; 
        glm::vec3 center() const;
        glm::vec3 min, max;
    };

    struct Sphere
    {
        Sphere();
        Sphere(glm::vec3 center, float radius);
        glm::vec3  center;
        float radius;
    };
   
  

    struct Frustum {
        Frustum() = default;
        explicit Frustum(mat4 const& vp);
        void ComputePlanes(mat4 const& vp);

        std::array<vec3, 6>  normals;
        std::array<float, 6> dists;
   
       
    };


    struct Ray
    {
        Ray();
        Ray(glm::vec3 origin, glm::vec3 direction);
        glm::vec3 origin;
        glm::vec3 direction;

        float IntersectionTime(const Aabb& aabb) const;
        bool TestRayAabb(const Aabb& aabb) const;
    //    bool RayCast(const Aabb& aabb, const Ray& ray, float& distance);

    };
}

#endif // __SHAPES_HPP__

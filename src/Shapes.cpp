
 /**
 * @file Shapes.cpp
 * @brief Implementation of the Aabb and Sphere classes
  * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
  * @date 6/6/2024
  * @copyright copyright (C) 2024 DigiPen Institute of Technology.
  */

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shapes.hpp"

namespace CS350 {

    
    Aabb::Aabb() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

  
    Aabb::Aabb(glm::vec3 minVal, glm::vec3 maxVal) : min(minVal), max(maxVal) {}

  
    float Aabb::SurfaceArea() const {
        glm::vec3 dimensions = max - min;
        return 2.0f * (dimensions.x * dimensions.y + dimensions.y * dimensions.z + dimensions.z * dimensions.x);
    }

    
    float Aabb::Volume() const {
        glm::vec3 dimensions = max - min;
        return dimensions.x * dimensions.y * dimensions.z;
    }


  
    Aabb Aabb::Transform(const glm::mat4x4& transform) {
        // Extract min components
        float minX = min.x;
        float minY = min.y;
        float minZ = min.z;

        // Extract max components
        float maxX = max.x;
        float maxY = max.y;
        float maxZ = max.z;

        // Initialize transformed min and max
        glm::vec3 tempMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 tempMax = glm::vec3(std::numeric_limits<float>::lowest());

        // Transform each corner of the AABB and update tempMin and tempMax
        for (int i = 0; i < 8; ++i) {
            // Calculate corner position
            glm::vec3 corner = glm::vec3((i & 1) != 0 ? maxX : minX,
                (i & 2) != 0 ? maxY : minY,
                (i & 4) != 0 ? maxZ : minZ);

            // Transform corner position
            glm::vec4 transformedCorner = transform * glm::vec4(corner, 1.0f);

            // Update tempMin and tempMax
            tempMin = glm::min(tempMin, glm::vec3(transformedCorner));
            tempMax = glm::max(tempMax, glm::vec3(transformedCorner));
        }

        // Update min and max of the AABB
        min = tempMin;
        max = tempMax;

        return *this;
    }

    Aabb Aabb::GetBoundingBox() const {
        return *this;
    }

    /**
     * @brief Default constructor for Sphere class.
     */
    Sphere::Sphere() : center(glm::vec3(0.0f)), radius(0.0f) {}

    /**
     * @brief Constructor for Sphere class with specified center and radius.
     * @param centerVal The center point of the sphere.
     * @param radiusVal The radius of the sphere.
     */
    Sphere::Sphere(glm::vec3 centerVal, float radiusVal) : center(centerVal), radius(radiusVal) {}


    Frustum::Frustum(mat4 const& vp)
    {
        ComputePlanes(vp);
    }

    void Frustum::ComputePlanes(mat4 const& vp)
    {
        mat4 const& vp_transposed = glm::transpose(vp);

        // Left clipping plane
        normals[0] = vp_transposed[3] + vp_transposed[0];
        dists[0] = vp_transposed[3][3] + vp_transposed[0][3];
        // Right clipping plane
        normals[1] = vp_transposed[3] - vp_transposed[0];
        dists[1] = vp_transposed[3][3] - vp_transposed[0][3];
        // Top clipping plane
        normals[2] = vp_transposed[3] + vp_transposed[1];
        dists[2] = vp_transposed[3][3] + vp_transposed[1][3];
        // Bottom clipping plane
        normals[3] = vp_transposed[3] - vp_transposed[1];
        dists[3] = vp_transposed[3][3] - vp_transposed[1][3];
        // Near clipping plane
        normals[4] = vp_transposed[3] + vp_transposed[2];
        dists[4] = vp_transposed[3][3] + vp_transposed[2][3];
        // Far clipping plane
        normals[5] = vp_transposed[3] - vp_transposed[2];
        dists[5] = vp_transposed[3][3] - vp_transposed[2][3];

        for (int i = 0; i < 6; ++i)
        {
            normals[i] *= -1;
            float length = glm::length(normals[i]);
            if (length > glm::epsilon<float>())
            {
                normals[i] /= length;
                dists[i] /= length;
            }
        }
    }

    //SideResult Frustum::TestNaive(Aabb const& bv) const
    //{
    //    Stats::Instance().frustumVsAabb++;
    //    return ClassifyFrustumAabbNaive(normals.data(), dists.data(), bv.min, bv.max);
    //}

  
  
    Ray::Ray() : origin(glm::vec3(0.0f)), direction(glm::vec3(0.0f, 0.0f, -1.0f)) {}

  
    Ray::Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(glm::normalize(direction)) {}

    
    float Ray::IntersectionTime(const Aabb& aabb) const {
        float tMin = (aabb.min.x - origin.x) / direction.x;
        float tMax = (aabb.max.x - origin.x) / direction.x;
        if (tMin > tMax) std::swap(tMin, tMax);

        float tyMin = (aabb.min.y - origin.y) / direction.y;
        float tyMax = (aabb.max.y - origin.y) / direction.y;
        if (tyMin > tyMax) std::swap(tyMin, tyMax);

        if ((tMin > tyMax) || (tyMin > tMax)) return -1;
        if (tyMin > tMin) tMin = tyMin;
        if (tyMax < tMax) tMax = tyMax;

        float tzMin = (aabb.min.z - origin.z) / direction.z;
        float tzMax = (aabb.max.z - origin.z) / direction.z;
        if (tzMin > tzMax) std::swap(tzMin, tzMax);

        if ((tMin > tzMax) || (tzMin > tMax)) return -1;
        if (tzMin > tMin) tMin = tzMin;
        if (tzMax < tMax) tMax = tzMax;

        return tMin > 0 ? tMin : tMax;
    }


    bool Ray::TestRayAabb(const Aabb& aabb) const {
        float tMin = (aabb.min.x - origin.x) / direction.x;
        float tMax = (aabb.max.x - origin.x) / direction.x;
        if (tMin > tMax) std::swap(tMin, tMax);

        float tyMin = (aabb.min.y - origin.y) / direction.y;
        float tyMax = (aabb.max.y - origin.y) / direction.y;
        if (tyMin > tyMax) std::swap(tyMin, tyMax);

        if ((tMin > tyMax) || (tyMin > tMax))
            return false;

        if (tyMin > tMin)
            tMin = tyMin;

        if (tyMax < tMax)
            tMax = tyMax;

        float tzMin = (aabb.min.z - origin.z) / direction.z;
        float tzMax = (aabb.max.z - origin.z) / direction.z;
        if (tzMin > tzMax) std::swap(tzMin, tzMax);

        if ((tMin > tzMax) || (tzMin > tMax))
            return false;

        return true;
    }

   
    glm::vec3 Aabb::center() const {
        return (min + max) * 0.5f;
    }


    //bool Ray::RayCast(const Aabb& aabb, const Ray& ray, float& distance) {
    //    if (!ray.TestRayAabb(aabb)) {
    //        return false;
    //    }

    //    // Calculate the intersection distance
    //    float tMin = (aabb.min.x - ray.origin.x) / ray.direction.x;
    //    float tMax = (aabb.max.x - ray.origin.x) / ray.direction.x;
    //    if (tMin > tMax) std::swap(tMin, tMax);

    //    float tyMin = (aabb.min.y - ray.origin.y) / ray.direction.y;
    //    float tyMax = (aabb.max.y - ray.origin.y) / ray.direction.y;
    //    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    //    if ((tMin > tyMax) || (tyMin > tMax)) return false;
    //    if (tyMin > tMin) tMin = tyMin;
    //    if (tyMax < tMax) tMax = tyMax;

    //    float tzMin = (aabb.min.z - ray.origin.z) / ray.direction.z;
    //    float tzMax = (aabb.max.z - ray.origin.z) / ray.direction.z;
    //    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    //    if ((tMin > tzMax) || (tzMin > tMax)) return false;
    //    if (tzMin > tMin) tMin = tzMin;
    //    if (tzMax < tMax) tMax = tzMax;

    //    distance = tMin > 0 ? tMin : tMax;
    //    return distance > 0;
    //}


}

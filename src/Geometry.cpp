/**
* @file Geometry.cpp
 * @brief Implementation of various geometric algorithms and functions
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 6/6/2024
 * @copyright copyright (C) 2024 DigiPen Institute of Technology.
 */


#include "Geometry.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp> 

namespace CS350 
{

    constexpr float cEpsilon = 1e-5f;

    /**
     * @brief Sets any NaN components of a vector to zero.
     * @param vec The vector to check for NaN components.
     */
    void avoidNans(vec3& vec)
    {
        if (glm::isnan(vec.x)) { vec.x = 0; }
        if (glm::isnan(vec.y)) { vec.y = 0; }
        if (glm::isnan(vec.z)) { vec.z = 0; }
    }

    /**
     * @brief Classifies a point with respect to an axis-aligned bounding box (AABB).
     * @param p The point to classify.
     * @param min The minimum point of the AABB.
     * @param max The maximum point of the AABB.
     * @return The classification of the point.
     */
    SideResult ClassifyPointAabb(vec3 const& p, vec3 const& min, vec3 const& max)
    {
        // Check if the point is overlapping, inside, or outside the AABB
        if (OverlapPointAabb(p, min, max))
        {
            // Check if the point is on any face of the AABB
            bool onFace = (p.x == min.x || p.x == max.x ||
                p.y == min.y || p.y == max.y ||
                p.z == min.z || p.z == max.z);

            return onFace ? eOVERLAPPING : eINSIDE;
        }
        return eOUTSIDE;
    }

    /**
     * @brief Classifies a point with respect to a plane.
     * @param n The normal vector of the plane.
     * @param d The distance from the origin to the plane.
     * @param p The point to classify.
     * @return The classification of the point.
     */
    SideResult ClassifyPlanePoint(const vec3& normal, float d, const vec3& point)
    {
        float distance = glm::dot(normal, point) + d;

        if (distance > 0)
        {
            return eINSIDE;
        }
        else if (distance < 0)
        {
            return eOUTSIDE;
        }
        else
        {
            return eOVERLAPPING;
        }
    }


    /**
     * @brief Classifies a triangle with respect to a plane.
     * @param n The normal vector of the plane.
     * @param d The distance from the origin to the plane.
     * @param A The first vertex of the triangle.
     * @param B The second vertex of the triangle.
     * @param C The third vertex of the triangle.
     * @return The classification of the triangle.
     */
    SideResult ClassifyPlaneTriangle(vec3 const& n, float d, vec3 const& A, vec3 const& B, vec3 const& C)
    {
        // Classify each vertex of the triangle
        SideResult tr1 = ClassifyPlanePoint(n, d, A);
        SideResult tr2 = ClassifyPlanePoint(n, d, B);
        SideResult tr3 = ClassifyPlanePoint(n, d, C);

        // Check for overlapping cases first
        if (tr1 == eOVERLAPPING || tr2 == eOVERLAPPING || tr3 == eOVERLAPPING)
        {
            return eOVERLAPPING;
        }

        // Check if all vertices are on the same side of the plane
        if (tr1 == tr2 && tr2 == tr3)
        {
            return tr1;
        }

        // If not all vertices are on the same side, return overlapping
        return eOVERLAPPING;
    }

    /**
     * @brief Classifies an axis-aligned bounding box (AABB) with respect to a plane.
     * @param n The normal vector of the plane.
     * @param d The distance from the origin to the plane.
     * @param min The minimum point of the AABB.
     * @param max The maximum point of the AABB.
     * @return The classification of the AABB.
     */
    SideResult ClassifyPlaneAabb(const vec3& normal, float d, const vec3& min, const vec3& max)
    {

        // Create the points of the AABB
        vec3 points[8] = {
            min,
            vec3{min.x, max.y, min.z},
            vec3{min.x, max.y, max.z},
            vec3{min.x, min.y, max.z},
            max,
            vec3{max.x, min.y, max.z},
            vec3{max.x, min.y, min.z},
            vec3{max.x, max.y, min.z}
        };

        SideResult result = eOVERLAPPING;
        for (int i = 0; i < 8; i++)
        {

            // Check every point
            SideResult point = ClassifyPlanePoint(normal, d, points[i]);


            // Don't take into account overlaps
            if (point == eOVERLAPPING) continue;

            // Save the result of the first valid point
            if (result == eOVERLAPPING)
                result = point;

            // Stop when result doesn't match
            else if (result != point)
                return eOVERLAPPING;
        }

        // If the loop finishes all the points are either in front or behind the plane
        return result;
    }


    /**
     * @brief Classifies a sphere with respect to a plane.
     * @param n The normal vector of the plane.
     * @param d The distance from the origin to the plane.
     * @param c The center of the sphere.
     * @param r The radius of the sphere.
     * @return The classification of the sphere.
     */
    SideResult ClassifyPlaneSphere(vec3 const& n, float d, vec3 const& c, float r)
    {
        float distance = glm::abs(glm::dot(n, c) - d) / glm::length(n);
        if (distance <= r)
        {
            return eOVERLAPPING;
        }
        return ClassifyPlanePoint(n, d, c);
    }

    /**
    * @brief Classifies a sphere with respect to a frustum.
    * @param fn An array of normal vectors representing the frustum planes.
    * @param fd An array of distances representing the frustum plane distances.
    * @param c The center of the sphere.
    * @param r The radius of the sphere.
    * @return The classification of the sphere.
    */
    SideResult ClassifyFrustumSphereNaive(vec3 const fn[6], float const fd[6], vec3 const& c, float r)
    {
        bool intersecting = false;
        for (int i = 0; i < 6; i++)
        {
            SideResult result = ClassifyPlaneSphere(fn[i], fd[i], c, r);
            if (result == eOUTSIDE)
            {
                return eOUTSIDE;
            }
            if (result == eOVERLAPPING)
            {
                intersecting = true;
            }
        }

        if (intersecting)
        {
            return eOVERLAPPING;
        }
        return eINSIDE;
    }

    /**
    * @brief Classifies an AABB with respect to a frustum.
    * @param fn An array of normal vectors representing the frustum planes.
    * @param fd An array of distances representing the frustum plane distances.
    * @param min The minimum point of the AABB.
    * @param max The maximum point of the AABB.
    * @return The classification of the AABB.
    */
    SideResult ClassifyFrustumAabbNaive(vec3 const fn[6], float const fd[6], vec3 const& min, vec3 const& max)
    {
        bool intersecting = false;

       /* std::cout << "Classifying Frustum AABB Naive" << std::endl;
        std::cout << "AABB Min: " << glm::to_string(min) << ", Max: " << glm::to_string(max) << std::endl;*/

        for (int i = 0; i < 6; i++)
        {
            //std::cout << "Testing plane " << i << " with normal: " << glm::to_string(fn[i]) << ", dist: " << fd[i] << std::endl;

            CS350::SideResult result = ClassifyPlaneAabb(fn[i], fd[i], min, max);

            if (result == CS350::SideResult::eOUTSIDE)
            {
               // std::cout << "AABB is outside of plane " << i << std::endl;
                return result;
            }

            if (result == CS350::SideResult::eOVERLAPPING)
            {
               // std::cout << "AABB intersects with plane " << i << std::endl;
                intersecting = true;
            }
        }

        if (intersecting)
        {
            //std::cout << "AABB is overlapping with the frustum" << std::endl;
            return CS350::SideResult::eOVERLAPPING;
        }

        //std::cout << "AABB is inside the frustum" << std::endl;
        return CS350::SideResult::eINSIDE;
    }


     /**
     * @brief Checks if a point overlaps with an AABB.
     * @param p The point to check.
     * @param min The minimum point of the AABB.
     * @param max The maximum point of the AABB.
     * @return True if the point overlaps with the AABB, false otherwise.
     */
    bool OverlapPointAabb(vec3 const& p, vec3 const& min, vec3 const& max)
    {
        return (p.x >= min.x && p.x <= max.x &&
            p.y >= min.y && p.y <= max.y &&
            p.z >= min.z && p.z <= max.z);
    }

     /**
     * @brief Checks if a point overlaps with a sphere.
     * @param p The point to check.
     * @param c The center of the sphere.
     * @param r The radius of the sphere.
     * @return True if the point overlaps with the sphere, false otherwise.
     */
    bool OverlapPointSphere(vec3 const& p, vec3 const& c, float r)
    {
        float distanceSquared = glm::dot(p - c, p - c);
        return distanceSquared <= r * r;
    }



     /**
     * @brief Checks if two AABBs overlap.
     * @param min1 The minimum point of the first AABB.
     * @param max1 The maximum point of the first AABB.
     * @param min2 The minimum point of the second AABB.
     * @param max2 The maximum point of the second AABB.
     * @return True if the AABBs overlap, false otherwise.
     */
    bool OverlapAabbAabb(vec3 const& min1, vec3 const& max1, vec3 const& min2, vec3 const& max2)
    {
        return min1.x <= max2.x && max1.x >= min2.x &&
            min1.y <= max2.y && max1.y >= min2.y &&
            min1.z <= max2.z && max1.z >= min2.z;
    }


    /**
     * @brief Checks if two spheres overlap.
     * @param c1 The center of the first sphere.
     * @param r1 The radius of the first sphere.
     * @param c2 The center of the second sphere.
     * @param r2 The radius of the second sphere.
     * @return True if the spheres overlap, false otherwise.
     */
    bool OverlapSphereSphere(vec3 const& c1, float r1, vec3 const& c2, float r2)
    {
        float distanceSquared = glm::dot(c1 - c2, c1 - c2);
        float sumRadiiSquared = (r1 + r2) * (r1 + r2);
        return distanceSquared <= sumRadiiSquared;
    }

     /**
     * @brief Computes the intersection time between a ray and a plane.
     * @param s The starting point of the ray.
     * @param dir The direction of the ray.
     * @param n The normal vector of the plane.
     * @param d The distance from the origin to the plane.
     * @return The time at which the ray intersects the plane, or -1 if no intersection.
     */
    float  IntersectionTimeRayPlane(vec3 const& s, vec3 const& dir, vec3 const& n, float d)
    {
        if (glm::abs(glm::dot(dir, n)) < cEpsilon)
        {
            return -1;
        }
        float t = -(glm::dot(n, s) - d) / glm::dot(n, dir);
        return t > 0 ? t : -1;
    }


    /**
     * @brief Computes the intersection time between a ray and an AABB.
     * @param s The starting point of the ray.
     * @param dir The direction of the ray.
     * @param min The minimum point of the AABB.
     * @param max The maximum point of the AABB.
     * @return The time at which the ray intersects the AABB, or -1 if no intersection.
     */
    float  IntersectionTimeRayAabb(vec3 const& s, vec3 const& dir, vec3 const& min, vec3 const& max)
    {
        float tx1 = (min.x - s.x) / dir.x;
        float tx2 = (max.x - s.x) / dir.x;
        float ty1 = (min.y - s.y) / dir.y;
        float ty2 = (max.y - s.y) / dir.y;
        float tz1 = (min.z - s.z) / dir.z;
        float tz2 = (max.z - s.z) / dir.z;

        float min_t = glm::max(glm::max(glm::min(tx1, tx2), glm::min(ty1, ty2)), glm::min(tz1, tz2));
        float max_t = glm::min(glm::min(glm::max(tx1, tx2), glm::max(ty1, ty2)), glm::max(tz1, tz2));

        if (max_t < 0 || min_t > max_t)
        {
            return -1;
        }
        // return intersection time
        return min_t > 0 ? min_t : 0;
    }

     /**
     * @brief Computes the intersection time between a ray and a sphere.
     * @param s The starting point of the ray.
     * @param dir The direction of the ray.
     * @param c The center of the sphere.
     * @param r The radius of the sphere.
     * @return The time at which the ray intersects the sphere, or -1 if no intersection.
     */
    float IntersectionTimeRaySphere(const vec3& s, const vec3& dir, const vec3& c, float r)
    {
        vec3 v = c - s;
        float t = glm::dot(v, dir);
        if (t < 0)
        {
            return -1;
        }
        vec3 closest = s + t * dir;

        float y = glm::length(c - closest);
        float x2 = r * r - y * y; 
        if (x2 < 0)
        {
            return -1;
        }
        float x = glm::sqrt(x2);

        // compute intersection values
        float t1 = t - x;
        return t1 >= 0 ? t1 : 0;
    }

    /**
     * @brief Computes the intersection time between a ray and a triangle.
     * @param s The starting point of the ray.
     * @param dir The direction of the ray.
     * @param p1 The first vertex of the triangle.
     * @param p2 The second vertex of the triangle.
     * @param p3 The third vertex of the triangle.
     * @return The time at which the ray intersects the triangle, or -1 if no intersection.
     */
    float IntersectionTimeRayTriangle(vec3 const& s, vec3 const& dir, vec3 const& p1, vec3 const& p2, vec3 const& p3)
    {
        vec3 ab = p2 - p1;
        vec3 ac = p3 - p1;
        vec3 normal = glm::normalize(glm::cross(ab, ac));
        float d = glm::dot(p1, normal);

        float t = IntersectionTimeRayPlane(s, dir, normal, d);
        if (t < 0)
        {
            return -1;
        }

        vec3 point = s + t * dir;
        if ((glm::dot(glm::cross(p2 - p1, point - p1), normal) < 0) ||
            (glm::dot(glm::cross(p3 - p2, point - p2), normal) < 0) ||
            (glm::dot(glm::cross(p1 - p3, point - p3), normal) < 0))
        {
            return -1;
        }
        // intersection is valid
        return t;
    }

    /**
     * @brief Computes the closest point on a plane to a given point.
     * @param point The point to which the closest point on the plane is computed.
     * @param normal The normal vector of the plane.
     * @param d The distance from the origin to the plane.
     * @return The closest point on the plane to the given point.
     */
    vec3 ClosestPointPlane(const vec3& point, const vec3& normal, float d)
    {
        float t = (glm::dot(point, normal) - d) / glm::length2(normal);
        return point - t * normal;
    }


     /**
     * @brief Computes the closest points between two line segments.
     * @param s1 The starting point of the first line segment.
     * @param e1 The ending point of the first line segment.
     * @param s2 The starting point of the second line segment.
     * @param e2 The ending point of the second line segment.
     * @param p1 Pointer to store the closest point on the first line segment.
     * @param p2 Pointer to store the closest point on the second line segment.
     */
    void ClosestSegmentSegment(vec3 const& s1, vec3 const& e1, vec3 const& s2, vec3 const& e2, vec3* p1, vec3* p2)
    {
        // intersection time and points
        float s = 0.f;
        float t = 0.f;
        vec3 P1{ 0.f };
        vec3 P2{ 0.f };
        // vectors
        vec3 k = s1 - s2;
        vec3 v = e1 - s1;
        vec3 w = e2 - s2;

        float a = glm::dot(v, v);
        float b = glm::dot(v, w);
        float c = glm::dot(w, w);
        float d = glm::dot(v, k);
        float e = glm::dot(w, k);

        // segments are parallel
        if (b * b - a * c == 0)
        {
            if (a == 0.f)
            {
                s = 0.f;
            }
            else
            {
                s = -d / a;
                s = glm::clamp(s, 0.f, 1.f);
            }
            if (c == 0.f)
            {
                t = 0.f;
            }
            else
            {
                t = (s * b + e) / c;
                t = glm::clamp(t, 0.f, 1.f);
            }
            P1 = s1 + s * v;
            P2 = s2 + t * w;
        }
        else
        {
            // compute t and clamp it
            t = (b * d - a * e) / (b * b - a * c);
            t = glm::clamp(t, 0.f, 1.f);
            // compute s now that you have t
            s = (-d + b * t) / a;
            s = glm::clamp(s, 0.f, 1.f);
            // recompute t in case that s was clamped
            t = (s * b + e) / c;
            t = glm::clamp(t, 0.f, 1.f);
            // compute intersection points for the segments
            P1 = s1 + s * v;
            P2 = s2 + t * w;
        }

        *p1 = P1;
        *p2 = P2;
    }

     /**
     * @brief Computes the axis-aligned bounding box (AABB) of a set of vertices using a brute force approach.
     * @param vertices Array of vertices.
     * @param count Number of vertices in the array.
     * @param out_min Pointer to store the minimum point of the computed AABB.
     * @param out_max Pointer to store the maximum point of the computed AABB.
     */
    void CreateAabbBruteForce(vec3 const* vertices, size_t count, vec3* out_min, vec3* out_max)
    {
        // Initialize min and max with the first vertex
        *out_min = *out_max = vertices[0];

        // Loop through the rest of the vertices searching for min and max
        for (size_t i = 1; i < count; ++i)
        {
            // Update min values
            if (vertices[i].x < out_min->x)
            {
                out_min->x = vertices[i].x;
            }
            if (vertices[i].y < out_min->y)
            {
                out_min->y = vertices[i].y;
            }
            if (vertices[i].z < out_min->z)
            {
                out_min->z = vertices[i].z;
            }

            // Update max values
            if (vertices[i].x > out_max->x)
            {
                out_max->x = vertices[i].x;
            }
            if (vertices[i].y > out_max->y)
            {
                out_max->y = vertices[i].y;
            }
            if (vertices[i].z > out_max->z)
            {
                out_max->z = vertices[i].z;
            }
        }
    }

    /**
     * @brief Computes the bounding sphere of a set of vertices using the centroid method.
     * @param vertices Array of vertices.
     * @param count Number of vertices in the array.
     * @param out_c Pointer to store the center of the computed sphere.
     * @param out_r Pointer to store the radius of the computed sphere.
     */
    void CreateSphereCentroid(vec3 const* vertices, size_t count, vec3* out_c, float* out_r)
    {
        // Compute centroid as average point
        vec3 centroid = vec3(0.0f, 0.0f, 0.0f);
        for (size_t i = 0; i < count; ++i)
        {
            centroid += vertices[i];
        }
        centroid /= static_cast<float>(count);

        // Find the maximum distance from centroid to any vertex
        float max_distance = 0.0f;
        for (size_t i = 0; i < count; ++i)
        {
            float distance = glm::distance(vertices[i], centroid);
            if (distance > max_distance)
            {
                max_distance = distance;
            }
        }

        *out_c = centroid;
        *out_r = max_distance;
    }


    /**
     * @brief Computes the bounding sphere of a set of vertices using Ritter's algorithm.
     * @param vertices Array of vertices.
     * @param count Number of vertices in the array.
     * @param out_c Pointer to store the center of the computed sphere.
     * @param out_r Pointer to store the radius of the computed sphere.
     */
    void CreateSphereRitters(vec3 const* vertices, size_t count, vec3* out_c, float* out_r)
    {
        // Compute the initial bounding box
        vec3 minCorner = vertices[0];
        vec3 maxCorner = vertices[0];
        for (size_t i = 1; i < count; ++i)
        {
            minCorner = glm::min(minCorner, vertices[i]);
            maxCorner = glm::max(maxCorner, vertices[i]);
        }
        vec3 center = (minCorner + maxCorner) * 0.5f;
        float radius = glm::distance(center, maxCorner);

        // Apply transformation to the bounding box (identity matrix as no transform provided)
        vec3 transformedCenter = center;
        float maxDistance = radius;

        // Check the rest of the points for the maximum distance
        for (size_t i = 0; i < count; ++i)
        {
            vec3 pos = vertices[i];
            float distance = glm::distance(transformedCenter, pos);
            if (distance > maxDistance)
            {
                // Expand the bounding sphere to encompass the point
                vec3 direction = glm::normalize(pos - transformedCenter);
                transformedCenter += (distance - maxDistance) * direction * 0.5f;
                maxDistance = (distance + maxDistance) * 0.5f;
            }
        }

        *out_c = transformedCenter;
        *out_r = maxDistance;
    }

      /**
     * @brief Computes the bounding sphere of a set of vertices using an iterative approach.
     * @param vertices Array of vertices.
     * @param count Number of vertices in the array.
     * @param iteration_count Number of iterations for the iterative algorithm.
     * @param shrink_ratio Ratio by which the bounding sphere shrinks in each iteration.
     * @param out_c Pointer to store the center of the computed sphere.
     * @param out_r Pointer to store the radius of the computed sphere.
     */
    void CreateSphereIterative(vec3 const* vertices, size_t count, int iteration_count, float shrink_ratio, vec3* out_c, float* out_r)
    {
        vec3 out_center = vec3(0.0f); 
        float out_radius = 0.0f; 
        CreateSphereRitters(vertices, count, &out_center, &out_radius);
        Sphere original_sphere(out_center, out_radius);

        Sphere sphere(original_sphere.center, original_sphere.radius);

        // Vector to shuffle positions
        std::vector<vec3> shuffled(vertices, vertices + count);

        // Iterate the given times
        for (int it = 0; it < iteration_count; ++it)
        {
            sphere.center = original_sphere.center;

            // Shrink the sphere in each iteration
            sphere.radius = original_sphere.radius - shrink_ratio;

            // Shuffle the vector to iterate randomly
            std::shuffle(shuffled.begin(), shuffled.end(), std::default_random_engine{});

            for (auto position : shuffled)
            {
                float distance = glm::distance(position, sphere.center);
                if (distance > sphere.radius)
                {
                    // Compute back of the sphere
                    vec3 direction = glm::normalize(position - sphere.center);
                    vec3 back = sphere.center + direction * sphere.radius;

                    // Expand center in the direction of position
                    sphere.center = (position + back) * 0.5f;

                    // Expand radius
                    sphere.radius = glm::distance(position, back) * 0.5f;
                }
            }

            // Update original sphere if a better one is found
            if (sphere.radius < original_sphere.radius)
            {
                original_sphere.center = sphere.center;
                original_sphere.radius = sphere.radius;
            }
        }

        *out_c = original_sphere.center;
        *out_r = original_sphere.radius;
    }

    /**
     * @brief Classifies whether two points are the same.
     *
     * @param point1 The first point.
     * @param point2 The second point.
     * @return True if the points are the same, false otherwise.
     */
    bool ClassifyPointPoint( vec3 point1,  vec3 point2)
    {
        return (point1 == point2);
    }


    /**
     * @brief Classifies whether a point lies on a segment.
     *
     * @param point The point to classify.
     * @param start The start of the segment.
     * @param end The end of the segment.
     * @return True if the point lies on the segment, false otherwise.
     */
    bool ClassifyPointSegment( vec3 point,  vec3 start,  vec3 end)
    {
        vec3 cross = glm::abs(glm::cross(point - start, end - start));
        if (cross.x < glm::epsilon<float>() && cross.y < glm::epsilon<float>() && cross.z < glm::epsilon<float>())
            return true;
        return false;
    }


    /**
     * @brief Classifies whether a point lies within a triangle.
     *
     * @param point The point to classify.
     * @param p1 The first vertex of the triangle.
     * @param p2 The second vertex of the triangle.
     * @param p3 The third vertex of the triangle.
     * @return True if the point lies within the triangle, false otherwise.
     */
    bool ClassifyPointTriangle( vec3 point,  vec3 p1,  vec3 p2,  vec3 p3)
    {
        // create the plane that contains the triangle
        vec3 v1 = p2 - p1;
        vec3 v2 = p3 - p1;
        vec3 normal = glm::cross(v1, v2);
        float d = glm::dot(normal, p1);
        // assert that the point lies on that plane of the triangle
        if (ClassifyPlanePoint(normal, d, point) == eOUTSIDE)
            return false;

        // check if the point is part of the triangle
        if ((glm::dot(glm::cross(p2 - p1, point - p1), normal) > 0) ||
            (glm::dot(glm::cross(p3 - p2, point - p2), normal) > 0) ||
            (glm::dot(glm::cross(p1 - p3, point - p3), normal) > 0))
            return false;
        // point is inside
        return true;
    }


    /**
     * @brief Classifies whether a point lies within a tetrahedron.
     *
     * @param point The point to classify.
     * @param p1 The first vertex of the tetrahedron.
     * @param p2 The second vertex of the tetrahedron.
     * @param p3 The third vertex of the tetrahedron.
     * @param p4 The fourth vertex of the tetrahedron.
     * @return True if the point lies within the tetrahedron, false otherwise.
     */
    bool ClassifyPointTetrahedron( vec3 point,  vec3& p1,  vec3& p2,  vec3& p3,  vec3& p4)
    {
        // divide the tetrahedron in 4 triangles and check if point is inside all of them 
        if (!ClassifyPointTriangle(point, p1, p2, p3) ||
            !ClassifyPointTriangle(point, p2, p3, p4) ||
            !ClassifyPointTriangle(point, p3, p4, p1) ||
            !ClassifyPointTriangle(point, p4, p1, p2))
            return false;
        // point inside all the planes of the tetrahedron
        return true;
    }
} // namespace CS350

/**
* @file Geometry.hpp
* @brief Implementation of the Aabb and Sphere classes
 * @author Teo Sheen Yeoh (t.sheenyeoh@digipen.edu)
 * @date 6/6/2024
 * @copyright copyright (C) 2024 DigiPen Institute of Technology.
 */


#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "Math.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include "Shapes.hpp"
#include <random>
#include <algorithm>

namespace CS350 {

    enum SideResult {
        eINSIDE      = -1,
        eOVERLAPPING = 0,
        eOUTSIDE     = 1
    };

    // Side classification
    SideResult ClassifyPointAabb(vec3 const& p, vec3 const& min, vec3 const& max);
    SideResult ClassifyPlanePoint(vec3 const& n, float d, vec3 const& p);
    SideResult ClassifyPlaneTriangle(vec3 const& n, float d, vec3 const& A, vec3 const& B, vec3 const& C);
    SideResult ClassifyPlaneAabb(vec3 const& n, float d, vec3 const& min, vec3 const& max);
    SideResult ClassifyPlaneSphere(vec3 const& n, float d, vec3 const& c, float r);
    SideResult ClassifyFrustumSphereNaive(vec3 const fn[6], float const fd[6], vec3 const& c, float r);
    SideResult ClassifyFrustumAabbNaive(vec3 const fn[6], float const fd[6], vec3 const& min, vec3 const& max);

    // Overlap tests
    bool OverlapPointAabb(vec3 const& p, vec3 const& min, vec3 const& max);
    bool OverlapPointSphere(vec3 const& p, vec3 const& c, float r);
    bool OverlapAabbAabb(vec3 const& min1, vec3 const& max1, vec3 const& min2, vec3 const& max2);
    bool OverlapSphereSphere(vec3 const& c1, float r1, vec3 const& c2, float r2);
    //bool OverlapSegmentPlane(vec3 const& s, vec3 const& e, vec3 const& n, float d);
    //bool OverlapSegmentTriangle(vec3 const& s, vec3 const& e, vec3 const& A, vec3 const& B, vec3 const& C);
    //bool OverlapSegmentAabb(vec3 const& s, vec3 const& e, vec3 const& min, vec3 const& max);
    //bool OverlapSegmentSphere(vec3 const& s, vec3 const& e, vec3 const& c, float r);

    // Intersection times
    float IntersectionTimeRayPlane(vec3 const& s, vec3 const& dir, vec3 const& n, float d);
    float IntersectionTimeRayTriangle(vec3 const& s, vec3 const& dir, vec3 const& p1, vec3 const& p2, vec3 const& p3);
    float IntersectionTimeRayAabb(vec3 const& s, vec3 const& dir, vec3 const& min, vec3 const& max);
    //float IntersectionTimeRayObb(vec3 const& s, vec3 const& dir, vec3 const& min, vec3 const& max, mat4 const& m2w);
    float IntersectionTimeRaySphere(vec3 const& s, vec3 const& dir, vec3 const& c, float r);
    //float IntersectionTimeSegmentPlane(vec3 const& s, vec3 const& e, vec3 const& n, float d);
    //float IntersectionTimeSegmentTriangle(vec3 const& s, vec3 const& e, vec3 const& A, vec3 const& B, vec3 const& C);
    //float IntersectionTimeSegmentAabb(vec3 const& s, vec3 const& e, vec3 const& min, vec3 const& max);
    //float IntersectionTimeSegmentSphere(vec3 const& s, vec3 const& e, vec3 const& c, float r);

    // Utils
    //bool  AreCollinear(vec3 const& a, vec3 const& b, vec3 const& c);
    //float DistancePointPlane(vec3 const& p, vec3 const& n, float d);
    //float DistanceSegmentPoint(vec3 const& s, vec3 const& e, vec3 const& pt);
    vec3  ClosestPointPlane(vec3 const& p, vec3 const& n, float d);
    void  ClosestSegmentSegment(vec3 const& s1, vec3 const& e1, vec3 const& s2, vec3 const& e2, vec3* p1, vec3* p2);
    //vec3  IntersectionPlanePlanePlane(vec3 const& n1, float d1, vec3 const& n2, float d2, vec3 const& n3, float d3);
    //void  TransformAabb(vec3 const& min, vec3 const& max, mat4 const& transform, vec3* out_min, vec3* out_max);
    //void  FrustumFromVp(mat4 const& vp, vec3 fn[6], float fd[6]);

    //// Geometry generation
    void CreateAabbBruteForce(vec3 const* vertices, size_t count, vec3* out_min, vec3* out_max);
    void CreateSphereCentroid(vec3 const* vertices, size_t count, vec3* out_c, float* out_r);
    void CreateSphereRitters(vec3 const* vertices, size_t count, vec3* out_c, float* out_r);
    void CreateSphereIterative(vec3 const* vertices, size_t count, int iteration_count, float shrink_ratio, vec3* out_c, float* out_r);

    //Aabb CreateAabbBruteForce(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    //Aabb CreateAabbFurthestPoint(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    //Sphere CreateSphereFurthestPoint(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    //Sphere CreateSphereCentroid(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    //Sphere CreateSphereRitter(const glm::vec3* positions, size_t size, const glm::mat4x4& transform = glm::mat4x4{ 1.f });
    //Sphere CreateSphereIterative(const glm::vec3* positions, size_t size, int iterations, float shrinkRatio, const glm::mat4x4& transform = glm::mat4x4{ 1.f });

    bool ClassifyPointPoint( vec3 point1,  vec3 point2);
    bool ClassifyPointSegment( vec3 point,  vec3 start,  vec3 end);
    bool ClassifyPointTetrahedron( vec3 point,  vec3& p1,  vec3& p2,  vec3& p3,  vec3& p4);
    bool ClassifyPointTriangle( vec3 point,  vec3 p1,  vec3 p2,  vec3 p3);


}

#endif // GEOMETRY_HPP

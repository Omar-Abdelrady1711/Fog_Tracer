#pragma once

#include "vec3.h"
#include "ray.h"

// Forward-declare Material so HitRecord can hold a pointer to it.
// Your teammate (Person 2) will define the full Material struct.
struct Material;

// =============================================================================
//  HitRecord — everything we know about a ray–object intersection
//
//  When a ray hits a surface, we fill one of these and pass it up the pipeline.
//  The renderer uses it for shading, shadow testing, and reflection.
// =============================================================================

struct HitRecord {
    Vec3      point;       // World-space hit position:  ray.at(t)
    Vec3      normal;      // Outward-facing unit normal at the hit point
    double    t    = 0.0;  // Ray parameter at which the hit occurred
    bool      frontFace;   // true if the ray hit the outside of the surface

    // Pointer to the surface material (owned by the object, not by HitRecord)
    const Material* material = nullptr;

    // -------------------------------------------------------------------------
    //  set_face_normal
    //
    //  Ensures `normal` always points against the incoming ray.
    //  Call this right after computing the geometric outward normal.
    //
    //  outwardNormal must already be unit-length.
    // -------------------------------------------------------------------------
    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = Vec3::dot(ray.direction, outwardNormal) < 0.0;
        normal    = frontFace ? outwardNormal : -outwardNormal;
    }
};

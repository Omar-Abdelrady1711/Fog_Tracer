#pragma once

#include "ray.h"
#include "hitrecord.h"

// =============================================================================
//  Object — abstract base class for anything a ray can hit
//
//  Every renderable object (Sphere, Plane, etc.) inherits from this and
//  implements hit().  The renderer only ever talks to Object*, so adding new
//  shapes later requires zero changes to the render loop.
// =============================================================================

class Object {
public:
    virtual ~Object() = default;

    // -------------------------------------------------------------------------
    //  hit
    //
    //  Test whether `ray` intersects this object in the interval (tMin, tMax).
    //
    //  Returns true and fills `rec` with intersection data if a hit is found.
    //  Returns false and leaves `rec` unchanged otherwise.
    //
    //  tMin  — minimum valid t (use a small epsilon like 1e-4 to avoid
    //          self-intersection / shadow acne)
    //  tMax  — maximum valid t (use infinity for primary rays)
    // -------------------------------------------------------------------------
    virtual bool hit(const Ray& ray,
                     double tMin,
                     double tMax,
                     HitRecord& rec) const = 0;
};

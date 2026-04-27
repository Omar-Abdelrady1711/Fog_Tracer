#pragma once

#include "vec3.h"

// =============================================================================
//  Ray — parametric ray:  P(t) = origin + t * direction
//
//  t > 0 means points "in front" of the origin (along the direction).
//  We use this everywhere: camera rays, shadow rays, reflection rays.
// =============================================================================

struct Ray {
    Vec3 origin;
    Vec3 direction;   // should be normalized before use

    // -------------------------------------------------------------------------
    //  Constructors
    // -------------------------------------------------------------------------
    Ray() = default;

    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction.normalized()) {}

    // -------------------------------------------------------------------------
    //  Evaluate P(t) = origin + t * direction
    // -------------------------------------------------------------------------
    Vec3 at(double t) const {
        return origin + direction * t;
    }
};

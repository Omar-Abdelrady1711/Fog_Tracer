#pragma once

#include "vec3.h"

// =============================================================================
//  Light — a point light source in the scene
//
//  Point lights emit equally in all directions from a single position.
//  Intensity falls off naturally via the Phong shading dot-product — we do
//  NOT apply quadratic attenuation here so the math stays simple and
//  controllable by the fog system (Person 3).
// =============================================================================

struct Light {
    Vec3 position;   // world-space location of the light bulb
    Vec3 color;      // RGB intensity  (values > 1 are valid for bright lights)

    // -------------------------------------------------------------------------
    //  Convenience factory
    // -------------------------------------------------------------------------
    static Light make(Vec3 position, Vec3 color = Vec3(1, 1, 1)) {
        return { position, color };
    }
};

#pragma once

#include "vec3.h"

// =============================================================================
//  Material — surface appearance properties used by Phong shading
//
//  Every Object holds one Material.  The renderer reads it when computing
//  the final pixel color at a hit point.
//
//  Phong model:
//    color = ambient + diffuse + specular
//          = Ka*Ia  +  Kd*(N·L)*Il  +  Ks*(R·V)^shininess*Il
// =============================================================================

struct Material {
    Vec3   ambient;       // Ka — base color even in shadow  (e.g. 0.1 * diffuse)
    Vec3   diffuse;       // Kd — main surface color
    Vec3   specular;      // Ks — highlight color (often white)
    double shininess;     // Phong exponent — higher = tighter highlight
    double reflectivity;  // 0 = no reflection, 1 = perfect mirror

    // -------------------------------------------------------------------------
    //  Convenience constructors
    // -------------------------------------------------------------------------

    // Plain matte surface
    static Material matte(Vec3 color, double ambientStrength = 0.1) {
        return {
            color * ambientStrength,   // ambient
            color,                     // diffuse
            Vec3(0, 0, 0),             // no specular
            1.0,
            0.0
        };
    }

    // Shiny plastic-like surface
    static Material shiny(Vec3 color, double shine = 64.0, double ambientStrength = 0.1) {
        return {
            color * ambientStrength,
            color,
            Vec3(1, 1, 1),             // white specular highlight
            shine,
            0.0
        };
    }

    // Reflective surface (metal / mirror)
    static Material reflective(Vec3 color, double reflectivity = 0.6, double shine = 128.0) {
        return {
            color * 0.05,
            color,
            Vec3(1, 1, 1),
            shine,
            reflectivity
        };
    }
};

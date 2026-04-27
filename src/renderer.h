#pragma once

#include <cmath>
#include <limits>
#include "vec3.h"
#include "ray.h"
#include "scene.h"
#include "hitrecord.h"
#include "material.h"

// =============================================================================
//  Renderer — turns a Ray into a color using Phong shading
//
//  Pipeline per ray:
//    1. Find closest hit in scene
//    2. For each light:
//       a. Cast shadow ray — skip light if blocked
//       b. Compute diffuse  (Lambertian:  Kd * max(N·L, 0))
//       c. Compute specular (Phong:       Ks * max(R·V, 0)^shininess)
//    3. Add ambient
//    4. If surface is reflective, recurse (up to maxDepth)
//    5. If no hit, return sky background color
// =============================================================================

static constexpr double T_MIN     = 1e-4;   // avoids self-intersection acne
static constexpr double T_MAX     = 1e9;
static constexpr int    MAX_DEPTH = 6;       // max reflection bounces

class Renderer {
public:
    // -------------------------------------------------------------------------
    //  rayColor — main entry point called once per pixel
    // -------------------------------------------------------------------------
    static Vec3 rayColor(const Ray& ray, const Scene& scene, int depth = 0) {
        // Recursion limit reached — return black (no more light contribution)
        if (depth >= MAX_DEPTH) return Vec3(0, 0, 0);

        HitRecord rec;
        if (!scene.hit(ray, T_MIN, T_MAX, rec)) {
            return skyColor(ray);
        }

        const Material& mat = *rec.material;
        Vec3 color(0, 0, 0);

        // Shading: loop over every light in the scene
        for (const Light& light : scene.lights) {
            Vec3   toLight    = (light.position - rec.point);
            double lightDist  = toLight.length();
            Vec3   L          = toLight / lightDist;   // unit vector toward light

            // ── Shadow ray ──────────────────────────────────────────────────
            // Cast a ray from the hit point toward the light.
            // If anything blocks it before reaching the light, skip shading.
            Ray    shadowRay(rec.point, L);
            HitRecord shadowRec;
            bool   inShadow = scene.hit(shadowRay, T_MIN, lightDist - T_MIN, shadowRec);
            if (inShadow) continue;

            // ── Diffuse (Lambertian) ─────────────────────────────────────────
            double NdotL   = std::max(0.0, Vec3::dot(rec.normal, L));
            Vec3   diffuse = mat.diffuse * NdotL;

            // ── Specular (Phong) ────────────────────────────────────────────
            Vec3   V       = (-ray.direction).normalized();           // toward camera
            Vec3   R       = Vec3::reflect(-L, rec.normal);           // reflected light
            double RdotV   = std::max(0.0, Vec3::dot(R, V));
            Vec3   specular = mat.specular * std::pow(RdotV, mat.shininess);

            // Accumulate this light's contribution
            color += (diffuse + specular) * light.color;
        }

        // ── Ambient ─────────────────────────────────────────────────────────
        color += mat.ambient;

        // ── Reflection ──────────────────────────────────────────────────────
        if (mat.reflectivity > 0.0) {
            Vec3 reflectDir = Vec3::reflect(ray.direction, rec.normal);
            Ray  reflectRay(rec.point, reflectDir);
            Vec3 reflectColor = rayColor(reflectRay, scene, depth + 1);
            color = color * (1.0 - mat.reflectivity) + reflectColor * mat.reflectivity;
        }

        return color;
    }

private:
    // ── Sky gradient — white horizon, steel-blue zenith ─────────────────────
    static Vec3 skyColor(const Ray& ray) {
        double t = 0.5 * (ray.direction.y + 1.0);
        return Vec3(1.0, 1.0, 1.0) * (1.0 - t) + Vec3(0.5, 0.7, 1.0) * t;
    }
};

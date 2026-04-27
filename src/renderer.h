#pragma once

#include <cmath>
#include <limits>
#include "vec3.h"
#include "ray.h"
#include "scene.h"
#include "hitrecord.h"
#include "material.h"
#include "fog.h"

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
//    5. Apply fog attenuation + volumetric light beams
//    6. If no hit, return sky background color
// =============================================================================

static constexpr double T_MIN     = 1e-4;
static constexpr double T_MAX     = 1e9;
static constexpr int    MAX_DEPTH = 6;

class Renderer {
public:
    static Vec3 rayColor(const Ray& ray, const Scene& scene, int depth = 0, const Fog* fog = nullptr) {
        if (depth >= MAX_DEPTH) return Vec3(0, 0, 0);

        HitRecord rec;
        if (!scene.hit(ray, T_MIN, T_MAX, rec)) {
            Vec3 color = skyColor(ray);

        if (fog) {
            double backgroundDistance = fog->marchDistance;
            color = fog->applyAttenuation(color, backgroundDistance);
            color += fog->marchLight(ray, scene, backgroundDistance);
        }

        return color;
    }

        const Material& mat = *rec.material;
        Vec3 color(0, 0, 0);

        for (const Light& light : scene.lights) {
            Vec3   toLight    = (light.position - rec.point);
            double lightDist  = toLight.length();
            Vec3   L          = toLight / lightDist;

            Ray       shadowRay(rec.point, L);
            HitRecord shadowRec;
            bool      inShadow = scene.hit(shadowRay, T_MIN, lightDist - T_MIN, shadowRec);
            if (inShadow) continue;

            double NdotL    = std::max(0.0, Vec3::dot(rec.normal, L));
            Vec3   diffuse  = mat.diffuse * NdotL;

            Vec3   V        = (-ray.direction).normalized();
            Vec3   R        = Vec3::reflect(-L, rec.normal);
            double RdotV    = std::max(0.0, Vec3::dot(R, V));
            Vec3   specular = mat.specular * std::pow(RdotV, mat.shininess);

            color += (diffuse + specular) * light.color;
        }

        color += mat.ambient;

        if (mat.reflectivity > 0.0) {
            Vec3 reflectDir   = Vec3::reflect(ray.direction, rec.normal);
            Ray  reflectRay(rec.point, reflectDir);
            Vec3 reflectColor = rayColor(reflectRay, scene, depth + 1, fog);
            color = color * (1.0 - mat.reflectivity) + reflectColor * mat.reflectivity;
        }

        // ── Fog ──────────────────────────────────────────────────────────────
        if (fog) {
            color = fog->applyAttenuation(color, rec.t);
            color += fog->marchLight(ray, scene, rec.t);
        }

        return color;
    }

private:
    static Vec3 skyColor(const Ray& ray) {
        double t = 0.5 * (ray.direction.y + 1.0);
        return Vec3(1.0, 1.0, 1.0) * (1.0 - t) + Vec3(0.5, 0.7, 1.0) * t;
    }
};
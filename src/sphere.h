#pragma once

#include <cmath>
#include "object.h"
#include "material.h"

// =============================================================================
//  Sphere — a perfectly round surface defined by center + radius
//
//  Ray–sphere intersection:
//    A point P is on the sphere if  |P - C|² = r²
//    Substituting P = O + t*D:
//      (D·D)t² + 2(D·(O-C))t + ((O-C)·(O-C) - r²) = 0
//    Solve the quadratic for t.  Two roots → entry & exit.
//    We want the smallest positive t inside (tMin, tMax).
// =============================================================================

class Sphere : public Object {
public:
    Vec3     center;
    double   radius;
    Material material;

    Sphere(Vec3 center, double radius, Material material)
        : center(center), radius(radius), material(material) {}

    bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override {
        Vec3   oc = ray.origin - center;
        double a  = Vec3::dot(ray.direction, ray.direction);
        double hb = Vec3::dot(ray.direction, oc);   // half b
        double c  = Vec3::dot(oc, oc) - radius * radius;

        double discriminant = hb * hb - a * c;
        if (discriminant < 0.0) return false;       // ray misses

        double sqrtD = std::sqrt(discriminant);

        // Try the nearer root first
        double t = (-hb - sqrtD) / a;
        if (t < tMin || t > tMax) {
            // Try the farther root (ray starts inside sphere)
            t = (-hb + sqrtD) / a;
            if (t < tMin || t > tMax) return false;
        }

        rec.t     = t;
        rec.point = ray.at(t);

        Vec3 outwardNormal = (rec.point - center) / radius;
        rec.setFaceNormal(ray, outwardNormal);
        rec.material = &material;

        return true;
    }
};

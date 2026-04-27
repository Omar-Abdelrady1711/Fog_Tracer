#pragma once

#include <cmath>
#include "object.h"
#include "material.h"

// =============================================================================
//  Plane — an infinite flat surface defined by a point + normal
//
//  Ray–plane intersection:
//    A point P is on the plane if  (P - Q) · N = 0
//    Substituting P = O + t*D:
//      t = ((Q - O) · N) / (D · N)
//    If D·N ≈ 0 the ray is parallel — no hit.
// =============================================================================

class Plane : public Object {
public:
    Vec3     point;     // any point lying on the plane
    Vec3     normal;    // unit normal (points "up" out of the surface)
    Material material;

    Plane(Vec3 point, Vec3 normal, Material material)
        : point(point), normal(normal.normalized()), material(material) {}

    bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override {
        double denom = Vec3::dot(ray.direction, normal);

        // Ray is (nearly) parallel to the plane — no intersection
        if (std::fabs(denom) < 1e-8) return false;

        double t = Vec3::dot(point - ray.origin, normal) / denom;
        if (t < tMin || t > tMax) return false;

        rec.t     = t;
        rec.point = ray.at(t);
        rec.setFaceNormal(ray, normal);
        rec.material = &material;

        return true;
    }
};

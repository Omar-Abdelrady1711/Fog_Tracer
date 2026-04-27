#pragma once

#include <algorithm>
#include <cmath>
#include "object.h"
#include "vec3.h"
#include "ray.h"
#include "hitrecord.h"
#include "material.h"

class Box : public Object {
public:
    Vec3 minCorner;
    Vec3 maxCorner;
    Material material;

    Box(const Vec3& minC, const Vec3& maxC, const Material& mat)
        : minCorner(minC), maxCorner(maxC), material(mat) {}

    bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override {
        double t0 = tMin;
        double t1 = tMax;
        Vec3 hitNormal(0, 0, 0);

        for (int axis = 0; axis < 3; ++axis) {
            double origin, direction, minA, maxA;

            if (axis == 0) {
                origin = ray.origin.x;
                direction = ray.direction.x;
                minA = minCorner.x;
                maxA = maxCorner.x;
            } else if (axis == 1) {
                origin = ray.origin.y;
                direction = ray.direction.y;
                minA = minCorner.y;
                maxA = maxCorner.y;
            } else {
                origin = ray.origin.z;
                direction = ray.direction.z;
                minA = minCorner.z;
                maxA = maxCorner.z;
            }

            if (std::abs(direction) < 1e-8) {
                if (origin < minA || origin > maxA) return false;
                continue;
            }

            double invD = 1.0 / direction;
            double nearT = (minA - origin) * invD;
            double farT  = (maxA - origin) * invD;

            Vec3 normalNear(0, 0, 0);

            if (axis == 0) normalNear = Vec3(invD >= 0 ? -1 : 1, 0, 0);
            if (axis == 1) normalNear = Vec3(0, invD >= 0 ? -1 : 1, 0);
            if (axis == 2) normalNear = Vec3(0, 0, invD >= 0 ? -1 : 1);

            if (nearT > farT) {
                std::swap(nearT, farT);
                normalNear = normalNear * -1.0;
            }

            if (nearT > t0) {
                t0 = nearT;
                hitNormal = normalNear;
            }

            t1 = std::min(t1, farT);
            if (t1 <= t0) return false;
        }

        rec.t = t0;
        rec.point = ray.origin + ray.direction * rec.t;
        rec.normal = hitNormal;
        rec.material = &material;

        return true;
    }
};
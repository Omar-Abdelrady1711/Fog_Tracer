#pragma once

#include <vector>
#include <memory>
#include <limits>
#include "object.h"
#include "light.h"
#include "hitrecord.h"
#include "ray.h"

// =============================================================================
//  Scene — container for all objects and lights
//
//  The renderer only talks to Scene.  It calls hit() to find the closest
//  surface along a ray, and reads lights for shading.
// =============================================================================

class Scene {
public:
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<Light>                   lights;

    // -------------------------------------------------------------------------
    //  Add helpers
    // -------------------------------------------------------------------------
    void add(std::shared_ptr<Object> obj)  { objects.push_back(obj); }
    void addLight(Light light)             { lights.push_back(light); }

    // -------------------------------------------------------------------------
    //  hit — find the closest object the ray intersects in (tMin, tMax)
    //
    //  Returns true and fills `rec` with the nearest hit.
    //  Returns false if nothing was hit.
    // -------------------------------------------------------------------------
    bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const {
        HitRecord tmp;
        bool      hitAnything = false;
        double    closest     = tMax;

        for (const auto& obj : objects) {
            if (obj->hit(ray, tMin, closest, tmp)) {
                hitAnything = true;
                closest     = tmp.t;
                rec         = tmp;
            }
        }

        return hitAnything;
    }
};

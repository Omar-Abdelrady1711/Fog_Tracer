#pragma once

#include <cmath>
#include <algorithm>
#include "vec3.h"
#include "ray.h"
#include "scene.h"

class Fog {
public:
    double density;
    double scattering;
    int marchSteps;
    double marchDistance;
    double anisotropy; // g: 0 = uniform, 0.7-0.9 = forward scattering

    Fog(double density = 0.08,
        double scattering = 1.5,
        int steps = 128,
        double distance = 20.0,
        double anisotropy = 0.85)
        : density(density),
          scattering(scattering),
          marchSteps(steps),
          marchDistance(distance),
          anisotropy(anisotropy) {}

    Vec3 applyAttenuation(const Vec3& surfaceColor, double distance) const {
        double attenuation = std::exp(-density * distance);

        // pale sky/fog color
        Vec3 fogColor(0.82, 0.88, 0.95);

        return surfaceColor * attenuation + fogColor * (1.0 - attenuation);
    }

    Vec3 marchLight(const Ray& ray, const Scene& scene, double hitDistance) const {
        double maxDist = std::min(hitDistance, marchDistance);
        if (maxDist <= 0.0) return Vec3(0, 0, 0);

        double stepSize = maxDist / marchSteps;
        Vec3 accumulated(0, 0, 0);

        double viewTransmittance = 1.0;

        for (int i = 0; i < marchSteps; ++i) {
            double t = (i + 0.5) * stepSize;
            if (t >= hitDistance) break;

            Vec3 samplePoint = ray.origin + ray.direction * t;

            for (const Light& light : scene.lights) {
                Vec3 toLight = light.position - samplePoint;
                double lightDist = toLight.length();
                Vec3 L = toLight / lightDist;

                Ray shadowRay(samplePoint, L);
                HitRecord shadowRec;
                bool blocked = scene.hit(shadowRay, 1e-4, lightDist - 1e-4, shadowRec);

                if (!blocked) {
                    double lightTransmittance = std::exp(-density * lightDist);

                    // Henyey-Greenstein phase function
                    double cosTheta = Vec3::dot(-ray.direction.normalized(), L);
                    double g = anisotropy;
                    double denom = 1.0 + g * g - 2.0 * g * cosTheta;
                    double phase = (1.0 - g * g) /
                        (4.0 * 3.141592653589793 * denom * std::sqrt(denom));

                    accumulated += light.color
                        * viewTransmittance
                        * lightTransmittance
                        * phase
                        * scattering
                        * density
                        * stepSize;
                }
            }

            viewTransmittance *= std::exp(-density * stepSize);
        }

        // Do not clamp too low; let beams show.
        accumulated.x = std::min(accumulated.x, 2.0);
        accumulated.y = std::min(accumulated.y, 2.0);
        accumulated.z = std::min(accumulated.z, 2.0);

        return accumulated;
    }
};
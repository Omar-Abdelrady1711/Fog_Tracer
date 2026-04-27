#pragma once

#include "ray.h"
#include "vec3.h"

// =============================================================================
//  Camera — perspective pinhole camera
//
//  Constructs a view frustum from human-friendly parameters (position,
//  look-at point, field-of-view) and generates one ray per pixel.
//
//  Coordinate convention:
//    +X  right
//    +Y  up
//    -Z  into the scene  (right-handed, OpenGL-style)
// =============================================================================

class Camera {
public:
    // -------------------------------------------------------------------------
    //  Constructor
    //
    //  lookFrom   — camera position in world space
    //  lookAt     — point the camera is aimed at
    //  vUp        — world "up" vector (usually {0,1,0})
    //  vFovDeg    — vertical field of view in degrees
    //  aspectRatio— image width / image height
    // -------------------------------------------------------------------------
    Camera(Vec3   lookFrom,
           Vec3   lookAt,
           Vec3   vUp,
           double vFovDeg,
           double aspectRatio)
    {
        // Convert vertical FOV to radians and compute viewport height
        double theta      = vFovDeg * (3.14159265358979323846 / 180.0);
        double halfHeight = std::tan(theta / 2.0);
        double halfWidth  = aspectRatio * halfHeight;

        // Build an orthonormal camera basis {w, u, v}
        //   w  — points from the scene BACK toward the camera  (-viewDir)
        //   u  — points right
        //   v  — points up (recomputed to be perpendicular to w)
        m_w = (lookFrom - lookAt).normalized();          // backward
        m_u = Vec3::cross(vUp, m_w).normalized();        // right
        m_v = Vec3::cross(m_w, m_u);                     // up (already unit)

        m_origin = lookFrom;

        // Lower-left corner of the virtual viewport (at focal distance = 1)
        m_lowerLeft = m_origin
                    - m_u * halfWidth
                    - m_v * halfHeight
                    - m_w;                // step 1 unit forward along -w

        m_horizontal = m_u * (2.0 * halfWidth);
        m_vertical   = m_v * (2.0 * halfHeight);
    }

    // -------------------------------------------------------------------------
    //  getRay
    //
    //  s, t ∈ [0, 1] are normalized pixel coordinates:
    //    s = (col + 0.5) / imageWidth
    //    t = (row + 0.5) / imageHeight   (row 0 = bottom)
    //
    //  Returns the ray that passes through that pixel.
    // -------------------------------------------------------------------------
    Ray getRay(double s, double t) const {
        Vec3 target = m_lowerLeft
                    + m_horizontal * s
                    + m_vertical   * t;
        return Ray(m_origin, target - m_origin);
    }

    // -------------------------------------------------------------------------
    //  Accessors (useful for debug output)
    // -------------------------------------------------------------------------
    Vec3 position()  const { return m_origin; }
    Vec3 right()     const { return m_u; }
    Vec3 up()        const { return m_v; }
    Vec3 backward()  const { return m_w; }

private:
    Vec3 m_origin;
    Vec3 m_lowerLeft;
    Vec3 m_horizontal;
    Vec3 m_vertical;
    Vec3 m_w, m_u, m_v;   // camera basis vectors
};

#pragma once

#include <cmath>
#include <iostream>

// =============================================================================
//  Vec3 — 3-component vector used for positions, directions, and colors
// =============================================================================

struct Vec3 {
    double x, y, z;

    // -------------------------------------------------------------------------
    //  Constructors
    // -------------------------------------------------------------------------
    Vec3()                          : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    // -------------------------------------------------------------------------
    //  Arithmetic operators
    // -------------------------------------------------------------------------
    Vec3  operator+ (const Vec3& v) const { return { x+v.x, y+v.y, z+v.z }; }
    Vec3  operator- (const Vec3& v) const { return { x-v.x, y-v.y, z-v.z }; }
    Vec3  operator* (double t)      const { return { x*t,   y*t,   z*t   }; }
    Vec3  operator/ (double t)      const { return { x/t,   y/t,   z/t   }; }
    Vec3  operator- ()              const { return {-x,    -y,    -z    }; }

    Vec3& operator+=(const Vec3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
    Vec3& operator*=(double t)      { x*=t;   y*=t;   z*=t;   return *this; }
    Vec3& operator/=(double t)      { x/=t;   y/=t;   z/=t;   return *this; }

    // Component-wise multiply (useful for color * color)
    Vec3 operator*(const Vec3& v) const { return { x*v.x, y*v.y, z*v.z }; }

    // -------------------------------------------------------------------------
    //  Length / normalization
    // -------------------------------------------------------------------------
    double lengthSquared() const { return x*x + y*y + z*z; }
    double length()        const { return std::sqrt(lengthSquared()); }

    Vec3 normalized() const {
        double len = length();
        return (len > 0.0) ? (*this / len) : Vec3(0, 0, 0);
    }

    // -------------------------------------------------------------------------
    //  Static math helpers
    // -------------------------------------------------------------------------
    static double dot(const Vec3& a, const Vec3& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y*b.z - a.z*b.y,
            a.z*b.x - a.x*b.z,
            a.x*b.y - a.y*b.x
        };
    }

    static Vec3 reflect(const Vec3& v, const Vec3& normal) {
        return v - normal * (2.0 * dot(v, normal));
    }

    // -------------------------------------------------------------------------
    //  Utility
    // -------------------------------------------------------------------------
    bool nearZero() const {
        const double eps = 1e-8;
        return (std::fabs(x) < eps) && (std::fabs(y) < eps) && (std::fabs(z) < eps);
    }

    // Clamp each component to [0, 1] — handy for colors before writing pixels
    Vec3 clamped() const {
        auto clamp = [](double v) { return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v); };
        return { clamp(x), clamp(y), clamp(z) };
    }

    // Debug print
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        return os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
    }
};

// Scalar on the left: 2.0 * v
inline Vec3 operator*(double t, const Vec3& v) { return v * t; }

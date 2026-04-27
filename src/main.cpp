#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cstdint>

#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "object.h"
#include "hitrecord.h"

// =============================================================================
//  Image settings
// =============================================================================
static constexpr int    IMAGE_WIDTH  = 800;
static constexpr int    IMAGE_HEIGHT = 450;
static constexpr double ASPECT_RATIO = static_cast<double>(IMAGE_WIDTH) / IMAGE_HEIGHT;

// =============================================================================
//  rayColor  —  what color does this ray produce?
//
//  Right now: a sky-gradient background (blue → white top → bottom).
//  Week 2: your teammates' Phong shading will replace/extend this.
// =============================================================================
Vec3 rayColor(const Ray& ray) {
    // Blend white (bottom) to sky-blue (top) based on the ray's Y direction
    double t = 0.5 * (ray.direction.y + 1.0);   // map [-1,1] → [0,1]
    Vec3 white(1.0, 1.0, 1.0);
    Vec3 skyBlue(0.5, 0.7, 1.0);
    return white * (1.0 - t) + skyBlue * t;
}

// =============================================================================
//  writePPM  —  save a flat RGB buffer to a .ppm file
//
//  PPM is a plain-text image format — no libraries needed, opens in VS Code
//  with the "PPM viewer" extension, or convert with IrfanView / GIMP.
// =============================================================================
void writePPM(const std::string& filename,
              const std::vector<Vec3>& pixels,
              int width, int height)
{
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "[error] could not open " << filename << "\n";
        return;
    }

    out << "P3\n" << width << " " << height << "\n255\n";

    for (int row = height - 1; row >= 0; --row) {   // flip: row 0 = bottom
        for (int col = 0; col < width; ++col) {
            Vec3 c = pixels[row * width + col].clamped();
            int r = static_cast<int>(255.999 * c.x);
            int g = static_cast<int>(255.999 * c.y);
            int b = static_cast<int>(255.999 * c.z);
            out << r << " " << g << " " << b << "\n";
        }
    }

    std::cout << "[info] saved " << filename << "  ("
              << width << "x" << height << ")\n";
}

// =============================================================================
//  main
// =============================================================================
int main() {
    // -------------------------------------------------------------------------
    //  Camera
    // -------------------------------------------------------------------------
    Camera camera(
        Vec3(0, 1, 3),    // lookFrom  — position
        Vec3(0, 0, 0),    // lookAt    — target
        Vec3(0, 1, 0),    // vUp
        45.0,             // vertical FOV (degrees)
        ASPECT_RATIO
    );

    // -------------------------------------------------------------------------
    //  Pixel buffer
    // -------------------------------------------------------------------------
    std::vector<Vec3> pixels(IMAGE_WIDTH * IMAGE_HEIGHT);

    // -------------------------------------------------------------------------
    //  Render loop
    //  For each pixel shoot one ray and store the returned color.
    //  Weeks 2–4 will replace rayColor() with full shading + fog.
    // -------------------------------------------------------------------------
    std::cout << "[info] rendering " << IMAGE_WIDTH << "x" << IMAGE_HEIGHT << "...\n";

    for (int row = 0; row < IMAGE_HEIGHT; ++row) {
        // Progress indicator
        if (row % 50 == 0)
            std::cout << "  row " << row << " / " << IMAGE_HEIGHT << "\r" << std::flush;

        for (int col = 0; col < IMAGE_WIDTH; ++col) {
            // Normalized pixel coordinates — center of each pixel
            double s = (col + 0.5) / IMAGE_WIDTH;
            double t = (row + 0.5) / IMAGE_HEIGHT;

            Ray   ray   = camera.getRay(s, t);
            Vec3  color = rayColor(ray);

            pixels[row * IMAGE_WIDTH + col] = color;
        }
    }

    std::cout << "\n";

    // -------------------------------------------------------------------------
    //  Output
    // -------------------------------------------------------------------------
    writePPM("output.ppm", pixels, IMAGE_WIDTH, IMAGE_HEIGHT);

    return 0;
}

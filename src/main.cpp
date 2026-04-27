#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "scene.h"
#include "renderer.h"
#include "sphere.h"
#include "plane.h"
#include "material.h"
#include "light.h"

// =============================================================================
//  Image / window settings
// =============================================================================
static constexpr int IMAGE_WIDTH  = 800;
static constexpr int IMAGE_HEIGHT = 450;
static constexpr double ASPECT_RATIO = static_cast<double>(IMAGE_WIDTH) / IMAGE_HEIGHT;

// Preview renders at half resolution for speed while moving
static constexpr int PREVIEW_SCALE = 3;   // divides resolution during movement
static constexpr int AA_SAMPLES    = 4;   // 2x2 grid AA for full render

// =============================================================================
//  writePPM
// =============================================================================
void writePPM(const std::string& filename,
              const std::vector<Vec3>& pixels,
              int width, int height)
{
    std::ofstream out(filename);
    out << "P3\n" << width << " " << height << "\n255\n";
    for (int row = height - 1; row >= 0; --row)
        for (int col = 0; col < width; ++col) {
            Vec3 c = pixels[row * width + col].clamped();
            out << (int)(255.999*c.x) << " "
                << (int)(255.999*c.y) << " "
                << (int)(255.999*c.z) << "\n";
        }
    std::cout << "[info] saved " << filename << "\n";
}

// =============================================================================
//  buildScene
// =============================================================================
Scene buildScene() {
    Scene scene;

    scene.add(std::make_shared<Plane>(
        Vec3(0,0,0), Vec3(0,1,0),
        Material::reflective(Vec3(0.7,0.7,0.75), 0.25, 32.0)));

    scene.add(std::make_shared<Sphere>(
        Vec3(0,1,-1), 1.0,
        Material::shiny(Vec3(0.85,0.2,0.15), 128.0)));

    scene.add(std::make_shared<Sphere>(
        Vec3(-2.2,0.7,-1.5), 0.7,
        Material::matte(Vec3(0.2,0.4,0.9))));

    scene.add(std::make_shared<Sphere>(
        Vec3(2.2,0.7,-1.5), 0.7,
        Material::reflective(Vec3(0.9,0.85,0.7), 0.8, 256.0)));

    scene.add(std::make_shared<Sphere>(
        Vec3(0.6,0.35,0.6), 0.35,
        Material::shiny(Vec3(0.95,0.95,0.9), 64.0)));

    scene.addLight(Light::make(Vec3(-3,6,2),  Vec3(1.0,0.95,0.85)));
    scene.addLight(Light::make(Vec3(4,3,1),   Vec3(0.25,0.3,0.4)));

    return scene;
}

// =============================================================================
//  Render into a pixel buffer
// =============================================================================
void renderToBuffer(std::vector<Vec3>& pixels,
                    int width, int height,
                    const Camera& cam,
                    const Scene& scene,
                    int samples)
{
    for (int row = 0; row < height; ++row)
        for (int col = 0; col < width; ++col) {
            Vec3 color(0,0,0);
            if (samples == 1) {
                double s = (col + 0.5) / width;
                double t = (row + 0.5) / height;
                color = Renderer::rayColor(cam.getRay(s, t), scene);
            } else {
                for (int sy = 0; sy < 2; ++sy)
                    for (int sx = 0; sx < 2; ++sx) {
                        double s = (col + (sx+0.5)*0.5) / width;
                        double t = (row + (sy+0.5)*0.5) / height;
                        color += Renderer::rayColor(cam.getRay(s, t), scene);
                    }
                color = color / (double)samples;
            }
            pixels[row * width + col] = color;
        }
}

// =============================================================================
//  main  —  SDL2 interactive viewer
// =============================================================================
#ifdef _WIN32
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#else
#include <SDL2/SDL.h>
#endif

int main(int argc, char* argv[]) {
    // ── SDL init ──────────────────────────────────────────────────────────────
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "[error] SDL_Init: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Fog Tracer  |  WASD=move  Mouse=look  QE=up/down  F=fullrender  ESC=quit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        IMAGE_WIDTH, IMAGE_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        IMAGE_WIDTH, IMAGE_HEIGHT);

    // Capture mouse for look-around
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // ── Scene ─────────────────────────────────────────────────────────────────
    Scene scene = buildScene();

    // ── Camera state ─────────────────────────────────────────────────────────
    Vec3   camPos(0, 2, 5);
    double yaw   = -90.0;   // degrees, horizontal rotation
    double pitch =  -15.0;  // degrees, vertical tilt
    double moveSpeed  = 0.15;
    double mouseSens  = 0.15;

    // Pixel buffers
    std::vector<Vec3>    pixels(IMAGE_WIDTH * IMAGE_HEIGHT);
    std::vector<uint8_t> rgb(IMAGE_WIDTH * IMAGE_HEIGHT * 3);

    bool needsRender  = true;
    bool fullQuality  = false;

    // ── Lambda: build Camera from current pos/yaw/pitch ──────────────────────
    auto makeCamera = [&]() {
        double yr = yaw   * (3.14159265358979 / 180.0);
        double pr = pitch * (3.14159265358979 / 180.0);
        Vec3 forward(
            std::cos(pr) * std::cos(yr),
            std::sin(pr),
            std::cos(pr) * std::sin(yr)
        );
        Vec3 target = camPos + forward;
        return Camera(camPos, target, Vec3(0,1,0), 40.0, ASPECT_RATIO);
    };

    // ── Lambda: copy Vec3 pixels → RGB bytes (SDL wants top-row first) ────────
    auto pixelsToRGB = [&](const std::vector<Vec3>& src, int w, int h) {
        for (int row = 0; row < h; ++row) {
            int srcRow = h - 1 - row;   // flip vertically
            for (int col = 0; col < w; ++col) {
                Vec3 c = src[srcRow * w + col].clamped();
                int idx = (row * w + col) * 3;
                rgb[idx+0] = (uint8_t)(255.999 * c.x);
                rgb[idx+1] = (uint8_t)(255.999 * c.y);
                rgb[idx+2] = (uint8_t)(255.999 * c.z);
            }
        }
    };

    // ── Main loop ─────────────────────────────────────────────────────────────
    bool running = true;
    SDL_Event event;
    const uint8_t* keys = SDL_GetKeyboardState(nullptr);

    while (running) {
        // ── Events ────────────────────────────────────────────────────────────
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                // F = force full-quality re-render and save
                if (event.key.keysym.sym == SDLK_f) {
                    fullQuality  = true;
                    needsRender  = true;
                }
            }

            // Mouse look
            if (event.type == SDL_MOUSEMOTION) {
                yaw   += event.motion.xrel * mouseSens;
                pitch -= event.motion.yrel * mouseSens;
                pitch  = std::max(-89.0, std::min(89.0, pitch));
                needsRender = true;
                fullQuality = false;
            }
        }

        // ── Keyboard movement ─────────────────────────────────────────────────
        SDL_PumpEvents();
        double yr = yaw * (3.14159265358979 / 180.0);

        Vec3 forward(std::cos(yr), 0, std::sin(yr));   // horizontal forward
        Vec3 right  = Vec3::cross(forward, Vec3(0,1,0)).normalized();

        if (keys[SDL_SCANCODE_W]) { camPos += forward * moveSpeed; needsRender = true; fullQuality = false; }
        if (keys[SDL_SCANCODE_S]) { camPos -= forward * moveSpeed; needsRender = true; fullQuality = false; }
        if (keys[SDL_SCANCODE_A]) { camPos -= right   * moveSpeed; needsRender = true; fullQuality = false; }
        if (keys[SDL_SCANCODE_D]) { camPos += right   * moveSpeed; needsRender = true; fullQuality = false; }
        if (keys[SDL_SCANCODE_E]) { camPos.y += moveSpeed;         needsRender = true; fullQuality = false; }
        if (keys[SDL_SCANCODE_Q]) { camPos.y -= moveSpeed;         needsRender = true; fullQuality = false; }

        // ── Render if needed ──────────────────────────────────────────────────
        if (needsRender) {
            Camera cam = makeCamera();

            if (fullQuality) {
                // Full resolution + AA
                pixels.resize(IMAGE_WIDTH * IMAGE_HEIGHT);
                renderToBuffer(pixels, IMAGE_WIDTH, IMAGE_HEIGHT, cam, scene, AA_SAMPLES);
                rgb.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
                pixelsToRGB(pixels, IMAGE_WIDTH, IMAGE_HEIGHT);

                // Save to disk too
                writePPM("output.ppm", pixels, IMAGE_WIDTH, IMAGE_HEIGHT);

                SDL_UpdateTexture(texture, nullptr, rgb.data(), IMAGE_WIDTH * 3);
                fullQuality = false;
            } else {
                // Low resolution preview
                int pw = IMAGE_WIDTH  / PREVIEW_SCALE;
                int ph = IMAGE_HEIGHT / PREVIEW_SCALE;
                std::vector<Vec3>    previewPx(pw * ph);
                std::vector<uint8_t> previewRGB(pw * ph * 3);

                renderToBuffer(previewPx, pw, ph, cam, scene, 1);

                // Scale back up to full window size (nearest neighbor)
                rgb.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
                for (int row = 0; row < IMAGE_HEIGHT; ++row)
                    for (int col = 0; col < IMAGE_WIDTH; ++col) {
                        int pr2 = (ph - 1 - (row * ph / IMAGE_HEIGHT));
                        int pc  = col * pw / IMAGE_WIDTH;
                        pr2 = std::max(0, std::min(ph-1, pr2));
                        pc  = std::max(0, std::min(pw-1, pc));
                        Vec3 c = previewPx[pr2 * pw + pc].clamped();
                        int idx = (row * IMAGE_WIDTH + col) * 3;
                        rgb[idx+0] = (uint8_t)(255.999 * c.x);
                        rgb[idx+1] = (uint8_t)(255.999 * c.y);
                        rgb[idx+2] = (uint8_t)(255.999 * c.z);
                    }

                SDL_UpdateTexture(texture, nullptr, rgb.data(), IMAGE_WIDTH * 3);
            }

            needsRender = false;
        }

        // ── Draw ──────────────────────────────────────────────────────────────
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, texture, nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer);
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
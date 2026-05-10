# Fog_Tracer

A C++17 recursive ray tracer designed to render volumetric fog and crepuscular rays (visible volumetric light beams). The project features an SDL2-based interactive viewer that allows real-time camera navigation and high-quality offline rendering.

## Features

- Phong shading
- Recursive reflections
- Volumetric fog
- Ray-marched light scattering
- Beer-Lambert attenuation
- Henyey-Greenstein forward scattering
- SDL2 interactive camera viewer
- Full-quality offline rendering mode

---

## Prerequisites

Install the following before building the project:

- Visual Studio 2022 with **Desktop development with C++**
- CMake 3.16+
- Git
- SDL2 installed through vcpkg

---

## Installation

### 1. Install vcpkg

If you do not already have vcpkg:

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
```

### 2. Install SDL2

```bash
vcpkg install sdl2:x64-windows
vcpkg integrate install
```

---

## Clone the Repository

```bash
git clone https://github.com/Omar-Abdelrady1711/Fog_Tracer.git
cd Fog_Tracer
```

---

## Building the Project

Open **Developer Command Prompt for VS 2022** and run:

```bash
mkdir build
cd build

cmake .. ^
 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
 -DVCPKG_TARGET_TRIPLET=x64-windows ^
 -DSDL2_DIR=C:/vcpkg/installed/x64-windows/share/sdl2

cmake --build . --config Release
```

---

## Running

Inside the build directory:

```bash
.\Release\raytracer.exe
```

An SDL2 window should open showing the interactive scene.

---

## Controls

| Input | Action |
|------|------|
| W / S | Move forward / backward |
| A / D | Move left / right |
| Q / E | Move down / up |
| Mouse | Look around |
| F | Full-quality render + save image |
| ESC | Quit |

---

## Output

Pressing `F` performs a full-quality render with anti-aliasing and saves:

```text
output.ppm
```

inside the build directory.




cmake_minimum_required(VERSION 3.16)
project(RayTracer LANGUAGES CXX)

# =============================================================================
#  Compiler settings
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable full optimizations for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Strict warnings to catch mistakes early
if(MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# =============================================================================
#  Sources
#  Add new .cpp files here as your teammates create them (e.g. renderer.cpp)
# =============================================================================
set(SOURCES
    src/main.cpp
    # src/renderer.cpp   <- uncomment when Person 3 adds this
)

add_executable(raytracer ${SOURCES})

# Make all headers in src/ visible without path prefixes
target_include_directories(raytracer PRIVATE src)

# =============================================================================
#  Output binary lands in the build folder
# =============================================================================
set_target_properties(raytracer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

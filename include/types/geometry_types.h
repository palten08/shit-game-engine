#pragma once

#include <stdint.h>

#include "vector_types.h"
#include "matrix_types.h"
#include "vertex_types.h"

/** @defgroup Geometry 3D Geometry Structures
 *  @{
 */

 /**
  * @brief A structure representing a single point in 3D space
  * 
  * 28 bytes
  */
typedef struct {
    Vector3f position; // 12 bytes
    Vector3f normal; // 12 bytes
    uint32_t color; // 4 bytes
} Point3D;

/**
 * @brief A structure representing a line segment in 3D space
 * 
 * 72 bytes
 */
typedef struct {
    Point3D start; // 28 bytes
    Point3D end; // 28 bytes
    float length; // 4 bytes
    uint32_t color; // 4 bytes
} Line3D;

/**
 * @brief A structure representing a triangle in 3D space
 * 
 * 88 bytes
 */
typedef struct {
    Point3D vertices[3]; // 84 bytes
    int vertex_indices[3]; // 12 bytes
    Vector3f normal; // 12 bytes
    uint32_t color; // 4 bytes
} Triangle3D;

/**
 * @brief A structure representing a quad in 3D space
 * 
 * 16 bytes
 */
typedef struct {
    Triangle3D *triangles; // 8 bytes
    uint32_t triangle_count; // 4 bytes
    uint32_t color; // 4 bytes
} Quad3D;

 /** @} */ // End of Geometry group
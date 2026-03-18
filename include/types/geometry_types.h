#pragma once

#include <stdint.h>

#include "vector_types.h"
#include "matrix_types.h"

/** @defgroup Geometry 3D Geometry Structures
 *  @{
 */

 /**
  * @brief A structure representing a single point in 3D space
  * 
  * 36 bytes
  */
typedef struct {
    RGBVector3f color; // 12 bytes
    Vector3f position; // 12 bytes
    Vector3f normal; // 12 bytes
    Vector2f uv; // 8 bytes
} Point3D;

/**
 * @brief A structure representing a line segment in 3D space
 * 
 * 88 bytes
 */
typedef struct {
    Point3D start; // 36 bytes
    Point3D end; // 36 bytes
    RGBVector3f color; // 12 bytes
    float length; // 4 bytes
} Line3D;

/**
 * @brief A structure representing a triangle in 3D space
 * 
 * 192 bytes
 */
typedef struct {
    Point3D vertices[3]; // 108 bytes
    RGBVector3f vertex_colors[3]; // 36 bytes
    RGBVector3f flat_shading_color; // 12 bytes
    int vertex_normal_indices[3]; // 12 bytes
    int vertex_indices[3]; // 12 bytes
    int uv_indices[3]; // 12 bytes
    Vector3f face_normal; // 12 bytes
} Triangle3D;

/**
 * @brief A structure representing a 3D mesh
 * 
 * 116 bytes
 */
typedef struct {
    char name[64]; // 64 bytes
    Triangle3D *triangles; // 8 bytes
    Vector3f *face_normals; // 8 bytes
    Vector3f *vertex_normals; // 8 bytes
    Vector2f *uv_coordinates; // 8 bytes
    float bounding_sphere_radius; // 4 bytes
    int id; // 4 bytes
    int material_id; // 4 bytes
    int triangle_count; // 4 bytes
    int vertex_count; // 4 bytes
    int face_normal_count; // 4 bytes
    int vertex_normal_count; // 4 bytes
    int uv_coordinate_count; // 4 bytes
} Mesh3D;

 /** @} */ // End of Geometry group
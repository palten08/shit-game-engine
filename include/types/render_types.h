#pragma once

#include "vector_types.h"
#include "material_types.h"

/** @defgroup Render Render Structures
 *  @{
 */

 /**
  * @brief A structure representing a renderable triangle
  * 
  * 48 bytes
  */
typedef struct {
    RGBVector3f vertex_colors[3]; // 36 bytes
    Vector3f world_space_vertex_normals[3]; // 36 bytes
    Vector3f world_space_vertex_positions[3]; // 36 bytes
    Vector2i screen_positions[3]; // 24 bytes
    Vector2f uv_coordinates[3]; // 24 bytes
    float depth_values[3]; // 12 bytes
    float perspective_w_values[3]; // 12 bytes
    RGBVector3f flat_shading_color; // 12 bytes
    Material *material; // 8 bytes
} RenderTriangle;

/**
 * @brief A structure representing a list of renderable triangles
 * 
 * 16 bytes
 */
typedef struct {
    RenderTriangle *triangles; // 8 bytes
    int triangle_count; // 4 bytes
} RenderList;

typedef struct {
    float *depth_values; // Dynamically allocated array of depth values for each pixel
    int width; // Width of the depth buffer (same as window width)
    int height; // Height of the depth buffer (same as window height)
} DepthBuffer;

 /** @} */ // End of Render Structures group
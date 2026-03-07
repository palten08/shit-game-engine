#pragma once

#include "vector_types.h"

#define MAX_RENDERABLE_TRIANGLES 16384

/** @defgroup Render Render Structures
 *  @{
 */

 /**
  * @brief A structure representing a renderable triangle
  * 
  * 28 bytes
  */
typedef struct {
    Vector2i screen_positions[3]; // 24 bytes
    float depth_values[3]; // 12 bytes
    uint32_t color; // 4 bytes
} RenderTriangle;

/**
 * @brief A structure representing a list of renderable triangles
 * 
 * 28736 bytes
 */
typedef struct {
    RenderTriangle triangles[MAX_RENDERABLE_TRIANGLES]; // 28672 bytes
    int triangle_count; // 4 bytes
} RenderList;

typedef struct {
    float *depth_values; // Dynamically allocated array of depth values for each pixel
    int width; // Width of the depth buffer (same as window width)
    int height; // Height of the depth buffer (same as window height)
} DepthBuffer;

 /** @} */ // End of Render Structures group
#pragma once

#include "types.h"

/**
 * @brief A structure representing the result of clipping a triangle against the view frustum.
 * 
 * 256 bytes
 */
typedef struct {
    Vector4f vertices[9]; // 144 bytes
    RGBVector3f vertex_colors[9]; // 108 bytes
    Vector3f vertex_normals[9]; // 108 bytes
    Vector3f world_space_vertex_positions[9]; // 108 bytes
    Vector2f uv_coordinates[9]; // 72 bytes
    int vertex_count; // 4 bytes
} ClippingResult;

ClippingResult clip_triangle(Vector4f clip_space_vertices[3], RGBVector3f vertex_colors[3], Vector3f vertex_normals[3], Vector3f world_space_vertex_positions[3], Vector2f uv_coordinates[3]);
bool is_sphere_in_frustum(Vector4f center_view, float radius, float fov, float aspect, float near, float far);
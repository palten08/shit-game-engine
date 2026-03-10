#pragma once

#include "types.h"

typedef struct {
    Vector4f vertices[9];
    int vertex_count;
} ClippingResult;

ClippingResult clip_triangle(Vector4f clip_space_vertices[3]);
bool is_sphere_in_frustum(Vector4f center_view, float radius, float fov, float aspect, float near, float far);
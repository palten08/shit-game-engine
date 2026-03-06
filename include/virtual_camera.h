#pragma once

#include "types.h"

/**
 * @brief A structure representing a virtual camera in 3D space, including its field of view, aspect ratio, and near and far clipping planes.
 * 
 * 80 bytes
 */
typedef struct {
    Matrix4 view_matrix; // 64 bytes
    Matrix4 perspective_projection_matrix; // 64 bytes
    Vector3f position; // 12 bytes
    Vector3f rotation; // 12 bytes
    float field_of_view; // 4-bytes
    float aspect_ratio; // 4-bytes
    float near_plane; // 4-bytes
    float far_plane; // 4-bytes
} VirtualCamera;

VirtualCamera initialize_virtual_camera(float aspect_ratio, float field_of_view, float near_plane_distance, float far_plane_distance, Vector3f initial_position, Vector3f initial_rotation);
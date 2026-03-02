#pragma once

#include "types.h"

VirtualCamera initialize_virtual_camera(int resolution_x, int resolution_y, float field_of_view, float near_plane_distance, float far_plane_distance, Vector3f initial_position);
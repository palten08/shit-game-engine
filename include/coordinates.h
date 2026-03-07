#pragma once

#include "types.h"

Vector2f convert_clip_space_to_normalized_device_coordinates(Vector4f clip_space_coords);
Vector2i convert_normalized_device_coordinates_to_screen_coordinates(Vector2f ndc_coords, int screen_width, int screen_height);

#pragma once

#include "types.h"


Quaternion quaternion_identity();
Quaternion quaternion_add(Quaternion quaternion_a, Quaternion quaternion_b);
Quaternion quaternion_multiply(Quaternion quaternion_a, Quaternion quaternion_b);
Quaternion quaternion_normalize(Quaternion quaternion);
Quaternion quaternion_multiply_by_scalar(Quaternion quaternion, float scalar);
Quaternion quaternion_conjugate(Quaternion quaternion);
Quaternion quaternion_from_axis_angle(float x, float y, float z, float angle);
Quaternion quaternion_from_euler_angles(float roll, float pitch, float yaw);
Matrix4 quaternion_to_matrix4(Quaternion quaternion);
Quaternion quaternion_spherical_linear_interpolation(Quaternion quaternion_a, Quaternion quaternion_b, float t);
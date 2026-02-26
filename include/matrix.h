#pragma once

#include "../include/types.h"

Matrix4 identity();
Matrix4 mat4_multiply(Matrix4 a, Matrix4 b);
Vector4f mat4_multiply_vec4(Matrix4 m, Vector4f v);
Matrix4 mat4_translation(float tx, float ty, float tz);
Matrix4 mat4_scaling(float sx, float sy, float sz);
Matrix4 mat4_rotation_x(float angle);
Matrix4 mat4_rotation_y(float angle);
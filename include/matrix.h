#pragma once

#include "../include/types.h"

Matrix4 mat4_identity();
Matrix4 mat4_multiply(Matrix4 a, Matrix4 b);
Vector4f mat4_multiply_vec4(Matrix4 m, Vector4f v);
Matrix4 mat4_create_translation_matrix(float tx, float ty, float tz);
Matrix4 mat4_create_scaling_matrix(float sx, float sy, float sz);
Matrix4 mat4_create_rotation_x_matrix(float angle);
Matrix4 mat4_create_rotation_y_matrix(float angle);
Matrix4 mat4_create_rotation_z_matrix(float angle);

Matrix3 mat3_identity();
Matrix3 mat3_multiply(Matrix3 a, Matrix3 b);
Vector3f mat3_multiply_vec3(Matrix3 m, Vector3f v);
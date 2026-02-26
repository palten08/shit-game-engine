#pragma once

#include "types.h"

Vector3f vec3f_add(Vector3f a, Vector3f b);
Vector3f vec3f_subtract(Vector3f a, Vector3f b);
Vector3f vec3f_multiply(Vector3f a, Vector3f b);
Vector3f vec3f_multiply_scalar(Vector3f a, float scalar);
Vector3f vec3f_divide_scalar(Vector3f a, float scalar);
Vector3f vec3f_divide(Vector3f a, Vector3f b);
float vec3f_get_length(Vector3f a);
Vector3f vec3f_normalize(Vector3f a);
float vec3f_dot_product(Vector3f a, Vector3f b);
Vector3f vec3f_cross_product(Vector3f a, Vector3f b);
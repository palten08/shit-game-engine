#include "../include/vector.h"
#include "../include/types.h"
#include <math.h>

/** Adds two 3D vectors component-wise. */
/** Arguments:
 *  - (Vector3f) a: The first vector.
 *  - (Vector3f) b: The second vector.
 *  - Returns: The resulting vector after addition.
 */
Vector3f vec3f_add(Vector3f a, Vector3f b) {
    Vector3f result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

/** Subtracts the second 3D vector from the first, component-wise. */
/** Arguments:
 *  - (Vector3f) a: The first vector.
 *  - (Vector3f) b: The second vector.
 *  - Returns: The resulting vector after subtraction.
 */
Vector3f vec3f_subtract(Vector3f a, Vector3f b) {
    Vector3f result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

/** Multiplies two 3D vectors component-wise. */
/** Arguments:
 *  - (Vector3f) a: The first vector.
 *  - (Vector3f) b: The second vector.
 *  - Returns: The resulting vector after multiplication.
 */
Vector3f vec3f_multiply(Vector3f a, Vector3f b) {
    Vector3f result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

/** Multiplies a 3D vector by a scalar. */
/** Can be used to scale a vector (or invert it). */
/** Arguments:
 *  - (Vector3f) a: The vector.
 *  - (float) scalar: The scalar value.
 *  - Returns: The resulting vector after multiplication.
 */
Vector3f vec3f_multiply_scalar(Vector3f a, float scalar) {
    Vector3f result;
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    result.z = a.z * scalar;
    return result;
}

/** Divides a 3D vector by a scalar. */
/** Can be used to scale a vector. */
/** Arguments:
 *  - (Vector3f) a: The vector.
 *  - (float) scalar: The scalar value.
 *  - Returns: The resulting vector after division.
 */
Vector3f vec3f_divide_scalar(Vector3f a, float scalar) {
    Vector3f result;
    result.x = a.x / scalar;
    result.y = a.y / scalar;
    result.z = a.z / scalar;
    return result;
}

/** Divides the first 3D vector by the second, component-wise. */
/** Arguments:
 *  - (Vector3f) a: The first vector.
 *  - (Vector3f) b: The second vector.
 *  - Returns: The resulting vector after division.
 */
Vector3f vec3f_divide(Vector3f a, Vector3f b) {
    Vector3f result;
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    result.z = a.z / b.z;
    return result;
}

/** Calculates the length (magnitude) of a 3D vector. */
/** Arguments:
 *  - (Vector3f) a: The vector.
 *  - Returns: The length of the vector.
 */
float vec3f_get_length(Vector3f a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

/** Normalizes a 3D vector, returning a unit vector in the same direction. */
/** Arguments:
 *  - (Vector3f) a: The vector to normalize.
 *  - Returns: The normalized vector.
 */
Vector3f vec3f_normalize(Vector3f a) {
    float length = vec3f_get_length(a);
    if (length == 0) {
        // Return a zero vector if the input vector has zero length to avoid division by zero.
        Vector3f zero_vector = {0.0f, 0.0f, 0.0f};
        return zero_vector;
    }
    return vec3f_divide_scalar(a, length);
}

/** Calculates the dot product of two 3D vectors. */
/** Arguments:
 *  - (Vector3f) a: The first vector.
 *  - (Vector3f) b: The second vector.
 *  - Returns: The dot product of the two vectors.
 */
float vec3f_dot_product(Vector3f a, Vector3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/** Calculates the cross product of two 3D vectors. */
/** Arguments:
 *  - (Vector3f) a: The first vector.
 *  - (Vector3f) b: The second vector.
 *  - Returns: The cross product of the two vectors.
 */
Vector3f vec3f_cross_product(Vector3f a, Vector3f b) {
    Vector3f result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

/** Converts a 3D vector to a 4D vector by adding a w component. */
/** Arguments:
 *  - (Vector3f) a: The 3D vector.
 *  - (float) w: The w component to add.
 *  - Returns: The resulting 4D vector.
 */
Vector4f vec3f_to_vec4f(Vector3f a, float w) {
    Vector4f result;
    result.x = a.x;
    result.y = a.y;
    result.z = a.z;
    result.w = w;
    return result;
}
#include <math.h>

#include "../include/quaternion_operations.h"
#include "../include/types.h"

/**
 * @brief Produces an identity quaternion representing no rotation
 * @return An identity quaternion
 */
Quaternion quaternion_identity() {
    Quaternion resultant_quaternion;
    resultant_quaternion.r = 1;
    resultant_quaternion.x = 0;
    resultant_quaternion.y = 0;
    resultant_quaternion.z = 0;
    return resultant_quaternion;
}

/**
 * @brief Adds two quaternions together and returns the result.
 * @param quaternion_a The first quaternion to add.
 * @param quaternion_b The second quaternion to add.
 * @return The result of adding the two quaternions together.
 */
Quaternion quaternion_add(Quaternion quaternion_a, Quaternion quaternion_b) {
    Quaternion resultant_quaternion;
    resultant_quaternion.r = quaternion_a.r + quaternion_b.r;
    resultant_quaternion.x = quaternion_a.x + quaternion_b.x;
    resultant_quaternion.y = quaternion_a.y + quaternion_b.y;
    resultant_quaternion.z = quaternion_a.z + quaternion_b.z;
    return resultant_quaternion;
}

/**
 * @brief Multiplies two quaternions together and returns the result.
 * @param quaternion_a The first quaternion to multiply.
 * @param quaternion_b The second quaternion to multiply.
 * @return The result of multiplying the two quaternions together.
 */
Quaternion quaternion_multiply(Quaternion quaternion_a, Quaternion quaternion_b) {
    Quaternion resultant_quaternion;
    resultant_quaternion.r = (quaternion_a.r * quaternion_b.r) - (quaternion_a.x * quaternion_b.x) - (quaternion_a.y * quaternion_b.y) - (quaternion_a.z * quaternion_b.z);
    resultant_quaternion.x = (quaternion_a.r * quaternion_b.x) + (quaternion_b.r * quaternion_a.x) + (quaternion_a.y * quaternion_b.z) - (quaternion_a.z * quaternion_b.y);
    resultant_quaternion.y = (quaternion_a.r * quaternion_b.y) + (quaternion_b.r * quaternion_a.y) + (quaternion_a.z * quaternion_b.x) - (quaternion_a.x * quaternion_b.z);
    resultant_quaternion.z = (quaternion_a.r * quaternion_b.z) + (quaternion_b.r * quaternion_a.z) + (quaternion_a.x * quaternion_b.y) - (quaternion_a.y * quaternion_b.x);
    return resultant_quaternion;
}

/**
 * @brief Normalizes a quaternion and returns the result.
 * @param quaternion The quaternion to normalize.
 * @return The normalized quaternion.
 */
Quaternion quaternion_normalize(Quaternion quaternion) {
    Quaternion resultant_quaternion;
    float magnitude = sqrtf((quaternion.r * quaternion.r) + (quaternion.x * quaternion.x) + (quaternion.y * quaternion.y) + (quaternion.z * quaternion.z));
    resultant_quaternion.r = quaternion.r / magnitude;
    resultant_quaternion.x = quaternion.x / magnitude;
    resultant_quaternion.y = quaternion.y / magnitude;
    resultant_quaternion.z = quaternion.z / magnitude;
    return resultant_quaternion;
}

/**
 * @brief Multiplies a quaternion by a scalar value and returns the result.
 * @param quaternion The quaternion to multiply.
 * @param scalar The scalar value to multiply the quaternion by.
 * @return The result of multiplying the quaternion by the scalar value.
 */
Quaternion quaternion_multiply_by_scalar(Quaternion quaternion, float scalar) {
    Quaternion resultant_quaternion;
    resultant_quaternion.r = quaternion.r * scalar;
    resultant_quaternion.x = quaternion.x * scalar;
    resultant_quaternion.y = quaternion.y * scalar;
    resultant_quaternion.z = quaternion.z * scalar;
    return resultant_quaternion;
}

/**
 * @brief Calculates the conjugate of a quaternion and returns the result.
 * @param quaternion The quaternion to calculate the conjugate of.
 * @return The conjugate of the quaternion.
 */
Quaternion quaternion_conjugate(Quaternion quaternion) {
    Quaternion resultant_quaternion;
    resultant_quaternion.r = quaternion.r;
    resultant_quaternion.x = -quaternion.x;
    resultant_quaternion.y = -quaternion.y;
    resultant_quaternion.z = -quaternion.z;
    return resultant_quaternion;
}

/**
 * @brief Creates a quaternion from an axis of rotation and an angle of rotation, and returns the result.
 * @param x The x component of the axis of rotation.
 * @param y The y component of the axis of rotation.
 * @param z The z component of the axis of rotation.
 * @param angle The angle of rotation in radians.
 * @return The quaternion representing the rotation around the specified axis by the specified angle.
 */
Quaternion quaternion_from_axis_angle(float x, float y, float z, float angle) {
    Quaternion resultant_quaternion;
    float half_angle = angle / 2.0f;
    float sin_half_angle = sinf(half_angle);
    resultant_quaternion.r = cosf(half_angle);
    resultant_quaternion.x = x * sin_half_angle;
    resultant_quaternion.y = y * sin_half_angle;
    resultant_quaternion.z = z * sin_half_angle;
    return quaternion_normalize(resultant_quaternion);
}

/**
 * @brief Creates a quaternion from Euler angles (roll, pitch, yaw) and returns the result.
 * @param roll The roll angle in radians.
 * @param pitch The pitch angle in radians.
 * @param yaw The yaw angle in radians.
 * @return The quaternion representing the rotation defined by the Euler angles.
 */
Quaternion quaternion_from_euler_angles(float roll, float pitch, float yaw) {
    Quaternion resultant_quaternion;
    float half_roll = roll / 2.0f;
    float half_pitch = pitch / 2.0f;
    float half_yaw = yaw / 2.0f;
    float sin_half_roll = sinf(half_roll);
    float cos_half_roll = cosf(half_roll);
    float sin_half_pitch = sinf(half_pitch);
    float cos_half_pitch = cosf(half_pitch);
    float sin_half_yaw = sinf(half_yaw);
    float cos_half_yaw = cosf(half_yaw);
    resultant_quaternion.r = (cos_half_roll * cos_half_pitch * cos_half_yaw) + (sin_half_roll * sin_half_pitch * sin_half_yaw);
    resultant_quaternion.x = (sin_half_roll * cos_half_pitch * cos_half_yaw) - (cos_half_roll * sin_half_pitch * sin_half_yaw);
    resultant_quaternion.y = (cos_half_roll * sin_half_pitch * cos_half_yaw) + (sin_half_roll * cos_half_pitch * sin_half_yaw);
    resultant_quaternion.z = (cos_half_roll * cos_half_pitch * sin_half_yaw) - (sin_half_roll * sin_half_pitch * cos_half_yaw);
    return quaternion_normalize(resultant_quaternion);
}

/**
 * @brief Converts a quaternion to a 4x4 rotation matrix and returns the result.
 * @param quaternion The quaternion to convert to a rotation matrix.
 * @return The 4x4 rotation matrix representing the same rotation as the quaternion.
 */
Matrix4 quaternion_to_matrix4(Quaternion quaternion) {
    Matrix4 resultant_matrix;
    float xx = quaternion.x * quaternion.x;
    float yy = quaternion.y * quaternion.y;
    float zz = quaternion.z * quaternion.z;
    float xy = quaternion.x * quaternion.y;
    float xz = quaternion.x * quaternion.z;
    float yz = quaternion.y * quaternion.z;
    float rx = quaternion.r * quaternion.x;
    float ry = quaternion.r * quaternion.y;
    float rz = quaternion.r * quaternion.z;

    resultant_matrix.m[0][0] = 1.0f - 2.0f * (yy + zz);
    resultant_matrix.m[0][1] = 2.0f * (xy - rz);
    resultant_matrix.m[0][2] = 2.0f * (xz + ry);
    resultant_matrix.m[0][3] = 0.0f;

    resultant_matrix.m[1][0] = 2.0f * (xy + rz);
    resultant_matrix.m[1][1] = 1.0f - 2.0f * (xx + zz);
    resultant_matrix.m[1][2] = 2.0f * (yz - rx);
    resultant_matrix.m[1][3] = 0.0f;

    resultant_matrix.m[2][0] = 2.0f * (xz - ry);
    resultant_matrix.m[2][1] = 2.0f * (yz + rx);
    resultant_matrix.m[2][2] = 1.0f - 2.0f * (xx + yy);
    resultant_matrix.m[2][3] = 0.0f;

    resultant_matrix.m[3][0] = 0.0f;
    resultant_matrix.m[3][1] = 0.0f;
    resultant_matrix.m[3][2] = 0.0f;
    resultant_matrix.m[3][3] = 1.0f;

    return resultant_matrix;
}

/**
 * @brief Performs spherical linear interpolation (slerp) between two quaternions and returns the result.
 * @param quaternion_a The first quaternion to interpolate from.
 * @param quaternion_b The second quaternion to interpolate to.
 * @param t The interpolation factor between 0.0f and 1.0f, where 0.0f returns quaternion_a and 1.0f returns quaternion_b.
 * @return The result of performing spherical linear interpolation between the two quaternions.
 */
Quaternion quaternion_spherical_linear_interpolation(Quaternion quaternion_a, Quaternion quaternion_b, float t) {
    Quaternion resultant_quaternion;
    float cos_half_theta = (quaternion_a.r * quaternion_b.r) + (quaternion_a.x * quaternion_b.x) + (quaternion_a.y * quaternion_b.y) + (quaternion_a.z * quaternion_b.z);
    if (cos_half_theta < 0.0f) {
        quaternion_b.r = -quaternion_b.r;
        quaternion_b.x = -quaternion_b.x;
        quaternion_b.y = -quaternion_b.y;
        quaternion_b.z = -quaternion_b.z;
        cos_half_theta = -cos_half_theta;
    }
    if (fabsf(cos_half_theta) >= 1.0f) {
        return quaternion_a;
    }
    float half_theta = acosf(cos_half_theta);
    float sin_half_theta = sqrtf(1.0f - cos_half_theta * cos_half_theta);
    if (fabsf(sin_half_theta) < 0.001f) {
        resultant_quaternion.r = (quaternion_a.r * 0.5f) + (quaternion_b.r * 0.5f);
        resultant_quaternion.x = (quaternion_a.x * 0.5f) + (quaternion_b.x * 0.5f);
        resultant_quaternion.y = (quaternion_a.y * 0.5f) + (quaternion_b.y * 0.5f);
        resultant_quaternion.z = (quaternion_a.z * 0.5f) + (quaternion_b.z * 0.5f);
        return resultant_quaternion;
    }
    float ratio_a = sinf((1 - t) * half_theta) / sin_half_theta;
    float ratio_b = sinf(t * half_theta) / sin_half_theta;
    resultant_quaternion.r = (quaternion_a.r * ratio_a) + (quaternion_b.r * ratio_b);
    resultant_quaternion.x = (quaternion_a.x * ratio_a) + (quaternion_b.x * ratio_b);
    resultant_quaternion.y = (quaternion_a.y * ratio_a) + (quaternion_b.y * ratio_b);
    resultant_quaternion.z = (quaternion_a.z * ratio_a) + (quaternion_b.z * ratio_b);
    return resultant_quaternion;
}
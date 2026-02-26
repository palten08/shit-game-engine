#include "../include/matrix.h"
#include "../include/types.h"

/** Creates an identity matrix. */
/** The identity matrix is a square matrix with ones on the main diagonal and zeros elsewhere. */
/** Multiplying it against a vector4 leaves the vector unchanged. */
/** Returns: A 4x4 identity matrix. */
Matrix4 identity() {
    Matrix4 result = {0};
    for (int i = 0; i < 4; i++) {
        result.m[i][i] = 1.0f;
    }
    return result;
}

/** Multiplies two 4x4 matrices. */
/** Arguments:
 *  - (Matrix4) a: The first matrix.
 *  - (Matrix4) b: The second matrix.
 *  - Returns: The resulting matrix after multiplication.
 */
Matrix4 mat4_multiply(Matrix4 a, Matrix4 b) {
    Matrix4 result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

/** Multiplies a 4x4 matrix with a 4D vector. */
/** This is used to apply transformations to vectors, such as translation, rotation, and scaling. */
/** Arguments:
 *  - (Matrix4) m: The matrix.
 *  - (Vector4f) v: The vector.
 *  - Returns: The resulting vector after multiplication.
*/
Vector4f mat4_multiply_vec4(Matrix4 m, Vector4f v) {
    Vector4f result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

/** Creates a translation matrix. */
/** A translation matrix is used to move objects in 3D space. */
/** The translation components are stored in the last column of the matrix. */
/** Arguments:
 *  - (float) tx: Translation along the x-axis.
 *  - (float) ty: Translation along the y-axis.
 *  - (float) tz: Translation along the z-axis.
 *  - Returns: The resulting translation matrix.
 */
Matrix4 mat4_translation(float tx, float ty, float tz) {
    Matrix4 result = identity();
    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;
    return result;
}

/** Creates a scaling matrix. */
/** A scaling matrix is used to scale objects in 3D space. */
/** The scaling factors are stored along the main diagonal of the matrix. */
/** Arguments:
 *  - (float) sx: Scaling factor along the x-axis.
 *  - (float) sy: Scaling factor along the y-axis.
 *  - (float) sz: Scaling factor along the z-axis.
 *  - Returns: The resulting scaling matrix.
 */
Matrix4 mat4_scaling(float sx, float sy, float sz) {
    Matrix4 result = {0};
    result.m[0][0] = sx;
    result.m[1][1] = sy;
    result.m[2][2] = sz;
    result.m[3][3] = 1.0f;
    return result;
}

/** Creates a rotation matrix around the X-axis. */
/** A rotation matrix is used to rotate objects in 3D space around a specific axis. */
/** Arguments:
 *  - (float) angle: The rotation angle in radians.
 *  - Returns: The resulting rotation matrix.
 */
Matrix4 mat4_rotation_x(float angle) {
    Matrix4 result = identity();
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    result.m[1][1] = cos_angle;
    result.m[1][2] = -sin_angle;
    result.m[2][1] = sin_angle;
    result.m[2][2] = cos_angle;
    return result;
}

/** Creates a rotation matrix around the Y-axis. */
/** A rotation matrix is used to rotate objects in 3D space around a specific axis. */
/** Arguments:
 *  - (float) angle: The rotation angle in radians.
 *  - Returns: The resulting rotation matrix.
 */
Matrix4 mat4_rotation_y(float angle) {
    Matrix4 result = identity();
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    result.m[0][0] = cos_angle;
    result.m[0][2] = sin_angle;
    result.m[2][0] = -sin_angle;
    result.m[2][2] = cos_angle;
    return result;
}

/** Creates a rotation matrix around the Z-axis. */
/** A rotation matrix is used to rotate objects in 3D space around a specific axis. */
/** Arguments:
 *  - (float) angle: The rotation angle in radians.
 *  - Returns: The resulting rotation matrix.
 */
Matrix4 mat4_rotation_z(float angle) {
    Matrix4 result = identity();
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    result.m[0][0] = cos_angle;
    result.m[0][1] = -sin_angle;
    result.m[1][0] = sin_angle;
    result.m[1][1] = cos_angle;
    return result;
}
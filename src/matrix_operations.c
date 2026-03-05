#include "../include/matrix_operations.h"
#include "../include/types.h"

/** Creates an identity matrix. */
/** The identity matrix is a square matrix with ones on the main diagonal and zeros elsewhere. */
/** Multiplying it against a vector4 leaves the vector unchanged. */
/** Returns: A 4x4 identity matrix. */
Matrix4 mat4_identity() {
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
Matrix4 mat4_create_translation_matrix(float tx, float ty, float tz) {
    Matrix4 result = mat4_identity();
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
Matrix4 mat4_create_scaling_matrix(float sx, float sy, float sz) {
    Matrix4 result = mat4_identity();
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
Matrix4 mat4_create_rotation_x_matrix(float angle) {
    Matrix4 result = mat4_identity();
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
Matrix4 mat4_create_rotation_y_matrix(float angle) {
    Matrix4 result = mat4_identity();
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
Matrix4 mat4_create_rotation_z_matrix(float angle) {
    Matrix4 result = mat4_identity();
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    result.m[0][0] = cos_angle;
    result.m[0][1] = -sin_angle;
    result.m[1][0] = sin_angle;
    result.m[1][1] = cos_angle;
    return result;
}

/**
 * @brief Creates a perspective projection matrix.
 * A perspective projection matrix is used to create a sense of depth in 3D graphics by simulating the way objects appear smaller as they get further away from the viewer.
 * @param fov The field of view angle in radians.
 * @param aspect_ratio The aspect ratio of the viewport (width divided by height).
 * @param near_plane The distance to the near clipping plane.
 * @param far_plane The distance to the far clipping plane.
 * @return The resulting perspective projection matrix.
 */
Matrix4 mat4_create_perspective_projection_matrix(float fov, float aspect_ratio, float near_plane, float far_plane) {
    Matrix4 result = {0};
    float f = 1.0f / tanf(fov / 2.0f);
    result.m[0][0] = f / aspect_ratio;
    result.m[1][1] = f;
    result.m[2][2] = (far_plane + near_plane) / (near_plane - far_plane);
    result.m[2][3] = (2.0f * far_plane * near_plane) / (near_plane - far_plane);
    result.m[3][2] = -1.0f;
    return result;
}

/**
 *  @brief Creates an identity matrix for 3x3 matrices.
 * The identity matrix is a square matrix with ones on the main diagonal and zeros elsewhere.
 * @return A 3x3 identity matrix.
*/
Matrix3 mat3_identity() {
    Matrix3 result = {0};
    for (int i = 0; i < 3; i++) {
        result.m[i][i] = 1.0f;
    }
    return result;
}

/**
 * @brief Multiplies two 3x3 matrices.
 * This function performs matrix multiplication, which is a fundamental operation in linear algebra used for combining transformations in 2D space.
 * @param a The first 3x3 matrix.
 * @param b The second 3x3 matrix.
 * @return The resulting 3x3 matrix after multiplication.
 */
Matrix3 mat3_multiply(Matrix3 a, Matrix3 b) {
    Matrix3 result = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

/**
 * @brief Multiplies a 3x3 matrix with a 3D vector.
 * This function applies the transformation represented by the matrix to the vector, which is commonly used in 2D graphics for operations like rotation, scaling, and translation (using homogeneous coordinates).
 * @param m The 3x3 matrix.
 * @param v The 3D vector to be transformed.
 * @return The resulting 3D vector after multiplication.
*/
Vector3f mat3_multiply_vec3(Matrix3 m, Vector3f v) {
    Vector3f result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z;
    return result;
}

/**
 * @brief Creates a translation matrix for 2D transformations using homogeneous coordinates.
 * This function generates a 3x3 matrix that can be used to translate points in 2D space. The translation components are stored in the last column of the matrix, allowing for easy combination with other transformations like rotation and scaling.
 * @param tx The translation distance along the x-axis.
 * @param ty The translation distance along the y-axis.
 * @return A 3x3 translation matrix.
*/
Matrix3 mat3_create_translation_matrix(float tx, float ty) {
    Matrix3 result = mat3_identity();
    result.m[0][2] = tx;
    result.m[1][2] = ty;
    return result;
}

/**
 * @brief Creates a scaling matrix for 2D transformations.
 * This function generates a 3x3 matrix that can be used to scale points in 2D space. The scaling factors are stored along the main diagonal of the matrix, allowing for easy combination with other transformations like rotation and translation.
 * @param sx The scaling factor along the x-axis.
 * @param sy The scaling factor along the y-axis.
 * @return A 3x3 scaling matrix.
*/
Matrix3 mat3_create_scaling_matrix(float sx, float sy) {
    Matrix3 result = mat3_identity();
    result.m[0][0] = sx;
    result.m[1][1] = sy;
    result.m[2][2] = 1.0f;
    return result;
}

/**
 * @brief Creates a rotation matrix for 2D transformations.
 * This function generates a 3x3 matrix that can be used to rotate points in 2D space. The rotation is performed around the origin (0,0) and the angle is specified in radians. The resulting matrix can be combined with other transformations like translation and scaling for complex transformations.
 * @param angle The rotation angle in radians.
 * @return A 3x3 rotation matrix.
*/
Matrix3 mat3_create_rotation_matrix(float angle) {
    Matrix3 result = mat3_identity();
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    result.m[0][0] = cos_angle;
    result.m[0][1] = -sin_angle;
    result.m[1][0] = sin_angle;
    result.m[1][1] = cos_angle;
    return result;
}
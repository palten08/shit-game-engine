#pragma once

/** @defgroup Matrices Matrix Structures
 *  @{
 */

/**
 * @brief A 4x4 matrix structure used for transformations in 3D space.
 * 
 * 64 bytes
 */
typedef struct {
    float m[4][4]; // 16 floats, 64 bytes
} Matrix4;

/**
 * @brief A 2x2 matrix structure used for transformations in 2D space.
 * 
 * 16 bytes
 */
typedef struct {
    float m[2][2]; // 4 floats, 16 bytes
} Matrix2;

/**
 * @brief A 3x3 matrix structure used for transformations in 2D space with homogeneous coordinates.
 * 
 * 36 bytes
 */
typedef struct {
    float m[3][3]; // 9 floats, 36 bytes
} Matrix3;

/** @} */ // End of Matrices group
#pragma once

/** @defgroup Vectors Vector Structures
 *  @{
 */

/** @struct Vector2i
 *
 * @brief A 2D integer vector structure used for screen coordinates.
 * 
 * 8 bytes
 */ 
typedef struct {
    /*@{*/
    int x; /**< The x-coordinate of the vector. (Horizontal component) | 4-bytes */
    int y; /**< The y-coordinate of the vector. (Vertical component) | 4-bytes */
    /*@{*/
} Vector2i;

/**
 * @brief A 2D floating-point vector structure used for mathematical operations, containing two 4-byte float members: x and y.
 * 
 * 8 bytes
*/
typedef struct {
    float x; // 4-bytes
    float y; // 4-bytes
} Vector2f;

/**
 * @brief A 3D floating-point vector structure used for mathematical operations, containing three 4-byte float members: x, y, and z.
 * 
 * 12 bytes
*/
typedef struct {
    float x; /*!< The x-coordinate of the vector. (Horizontal component) | 4-bytes */
    float y; /*!< The y-coordinate of the vector. (Vertical component) | 4-bytes */
    float z; /*!< The z-coordinate of the vector. (Depth component) | 4-bytes */
} Vector3f;

/**
 * @brief A 4D floating-point vector structure used for mathematical operations, containing four 4-byte float members: x, y, z, and w.
 * The w component is used for homogeneous coordinates, allowing for more complex transformations in 3D
 * 
 * 16 bytes
 */
typedef struct {
    float x; // 4-bytes
    float y; // 4-bytes
    float z; // 4-bytes
    float w; // 4-bytes
} Vector4f;

/** @} */ // End of Vectors group
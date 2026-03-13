#pragma once


/** @defgroup Rotations Rotation Structures
 *  @{
 */

/**
 * @brief A struct representing a Quaternion
 * 
 * 16 bytes
 */
typedef struct {
    float r; // 4 bytes
    float x; // 4 bytes
    float y; // 4 bytes
    float z; // 4 bytes
} Quaternion;

/** @} */ // End of Rotations group
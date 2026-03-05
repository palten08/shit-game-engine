#pragma once

#include <stdint.h>
#include "vector_types.h"

/** @defgroup Vertex Vertex Structures
 *  @{
 */

/**
 * @brief A structure representing a vertex, including its position and color.
 * This structure is used to define the properties of a vertex in 2D space
 * 
 * 12 bytes
*/
typedef struct {
    Vector2i position; // 8-bytes
    uint32_t color; // 4-bytes
    bool is_visible; // 1 byte
} VertexData2D;

/**
 * @brief A structure representing a vertex, including its position and color.
 * This structure is used to define the properties of a vertex in 3D space
 * 
 * 16 bytes
 */
typedef struct {
    Vector3f position; // 12-bytes
    uint32_t color; // 4-bytes
    bool is_visible; // 1 byte
} VertexData3D;

/** @} */ // End of Vertex group
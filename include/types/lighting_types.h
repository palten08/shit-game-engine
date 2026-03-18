#pragma once

#include <stdint.h>

#include "vector_types.h"

/** @defgroup Lighting Lighting Structures
 *  @{
 */

 /**
  * @brief A structure representing a global directional light
  * 
  * 24 bytes
  */
typedef struct {
    Vector3f direction; // 12 bytes
    RGBVector3f color; // 12 bytes
    float intensity; // 4 bytes
    float ambient_intensity; // 4 bytes
} DirectionalLight;

 /** @} */ // End of Lighting group
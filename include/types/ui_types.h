#pragma once

#include "vector_types.h"

/** @defgroup UI UI Structures
 *  @{
 */

typedef struct {
    Vector2i position;
    Vector2i size;
    RGBVector3f color;
    int border_thickness;
} UIRectangle;

/** @} */ // End of UI group
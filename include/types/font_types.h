#pragma once

#include "vector_types.h"

/** @defgroup Font Font Structures
 *  @{
 */

 /**
  * @brief A structure representing a single character from a font atlas image
  * 
  * 32 bytes
  */
typedef struct {
    Vector2i atlas_position; // 8 bytes
    Vector2i atlas_size; // 8 bytes
    Vector2i cursor_offset; // 8 bytes
    int xadvance; // 4 bytes
} FontCharacter;

typedef struct {
    FontCharacter characters[128]; // 4096 bytes
    void *pixel_data; // 8 bytes
    Vector2i image_dimensions; // 8 bytes
    int line_height; // 4 bytes
} BitmapFont;

 /** @} */ // End of Font group
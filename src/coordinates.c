#include "../include/coordinates.h"

/** Converts 2D screen space coordinates to 2D world space coordinates. */
/** Arguments:
 *  - (Vector2i) screen_pos: The 2D position in screen space.
 *  - (int) screen_width: The width of the screen.
 *  - (int) screen_height: The height of the screen.
 *  - Returns: The corresponding 2D position in world space.
 */
Vector2i vec2i_screen_space_to_world_space(Vector2i screen_pos, int screen_width, int screen_height) {
    return (Vector2i){
        .x = screen_pos.x - screen_width / 2,
        .y = (screen_pos.y - screen_height / 2) * -1
    };
}

#include "../include/coordinates.h"
#include "../include/types.h"

/**
 * @brief Converts clip space coordinates to normalized device coordinates (NDC) by performing perspective division.
 * This assumes you already have the coordinates in clip space computed
 * @param clip_space_coords The coordinates in clip space.
 * @return The coordinates in normalized device coordinates (NDC).
 */
Vector2f convert_clip_space_to_normalized_device_coordinates(Vector4f clip_space_coords) {
    Vector2f ndc;
    ndc.x = clip_space_coords.x / clip_space_coords.w;
    ndc.y = clip_space_coords.y / clip_space_coords.w;
    return ndc;
}

/**
 * @brief Converts normalized device coordinates (NDC) to screen coordinates.
 * @param ndc_coords The coordinates in normalized device coordinates (NDC).
 * @param screen_width The width of the screen in pixels.
 * @param screen_height The height of the screen in pixels.
 * @return The coordinates in screen space.
 */
Vector2i convert_normalized_device_coordinates_to_screen_coordinates(Vector2f ndc_coords, int screen_width, int screen_height) {
    Vector2i screen_coords;
    screen_coords.x = (int)((ndc_coords.x + 1.0f) * 0.5f * screen_width);
    screen_coords.y = (int)((1.0f - (ndc_coords.y + 1.0f) * 0.5f) * screen_height); // Invert the y-coordinate because screen coordinates typically have the origin at the top-left corner
    return screen_coords;
}
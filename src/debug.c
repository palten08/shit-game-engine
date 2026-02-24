#include "../include/debug.h"
#include "../include/types.h"

int draw_squares_at_window_corners(AppContext *app_context, int size, uint32_t color) {
    put_square_at_coordinates(app_context, 0, 0, size, color, TOP_LEFT);
    put_square_at_coordinates(app_context, app_context->window_resolution.x, 0, size, color, TOP_RIGHT);
    put_square_at_coordinates(app_context, 0, app_context->window_resolution.y, size, color, BOTTOM_LEFT);
    put_square_at_coordinates(app_context, app_context->window_resolution.x, app_context->window_resolution.y, size, color, BOTTOM_RIGHT);
    return 0;
}

int draw_square_at_screen_position(AppContext *app_context, Vector2i position, int size, uint32_t color, PixelAlignment alignment) {
    put_square_at_coordinates(app_context, position.x, position.y, size, color, alignment);
    return 0;
}
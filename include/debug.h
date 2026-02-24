#pragma once

#include "../include/types.h"
#include "../include/render.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int draw_squares_at_window_corners(AppContext *app_context, int size, uint32_t color);

int draw_square_at_screen_position(AppContext *app_context, Vector2i position, int size, uint32_t color, PixelAlignment alignment);
#pragma once

#include "../include/types.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int set_frame_buffer(AppContext *app_context, uint32_t color);

int clear_frame_buffer(AppContext *app_context);

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title);

int cleanup_sdl_components(AppContext *app_context);

int put_square_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, int size, uint32_t color, PixelAlignment alignment);

int draw_horizontal_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color);

int draw_vertical_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color);

int draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color);

int render_scene(AppContext *app_context, Scene *scene);
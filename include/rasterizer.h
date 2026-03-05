#pragma once

#include "types.h"
#include "app.h"
#include "scene.h"

int set_frame_buffer(AppContext *app_context, uint32_t color);

int clear_frame_buffer(AppContext *app_context);

int draw_pixel_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, uint32_t color);

int draw_horizontal_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color);

int draw_vertical_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color);

int draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color);

int write_scene_to_frame_buffer(AppContext *app_context, Scene *scene);
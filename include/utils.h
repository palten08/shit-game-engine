#pragma once

#include "types.h"

double get_delta_time(uint64_t *ticks_now, uint64_t *ticks_last);

double get_instantaneous_frame_rate(uint64_t *ticks_now, uint64_t *ticks_last);

uint32_t convert_rgbvec3f_to_hex_color(RGBVector3f color);
uint32_t convert_rgbavec4f_to_hex_color(RGBAVector4f color);
RGBVector3f convert_hex_color_to_rgbvec3f(uint32_t hex_color);
RGBAVector4f convert_hex_color_to_rgbavec4f(uint32_t hex_color);
void draw_pixel_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, RGBVector3f color);
void draw_horizontal_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color);
void draw_vertical_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color);
void draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color);
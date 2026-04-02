#include <stdlib.h>

#include "../include/utils.h"
#include "../include/app.h"

double get_delta_time(uint64_t *ticks_now, uint64_t *ticks_last) {
    *ticks_last = *ticks_now;
    *ticks_now = SDL_GetPerformanceCounter();
    return ((*ticks_now - *ticks_last) / (double)SDL_GetPerformanceFrequency() );
}

double get_instantaneous_frame_rate(uint64_t *ticks_now, uint64_t *ticks_last) {
    double delta_time = get_delta_time(ticks_now, ticks_last);
    return 1.0 / delta_time;
}

uint32_t convert_rgbvec3f_to_hex_color(RGBVector3f color) {
    uint32_t r = (uint32_t)(color.r * 255.0f) & 0xFF;
    uint32_t g = (uint32_t)(color.g * 255.0f) & 0xFF;
    uint32_t b = (uint32_t)(color.b * 255.0f) & 0xFF;
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

uint32_t convert_rgbavec4f_to_hex_color(RGBAVector4f color) {
    uint32_t r = (uint32_t)(color.r * 255.0f) & 0xFF;
    uint32_t g = (uint32_t)(color.g * 255.0f) & 0xFF;
    uint32_t b = (uint32_t)(color.b * 255.0f) & 0xFF;
    uint32_t a = (uint32_t)(color.a * 255.0f) & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

RGBVector3f convert_hex_color_to_rgbvec3f(uint32_t hex_color) {
    RGBVector3f color;
    color.r = (float)((hex_color >> 16) & 0xFF) / 255.0f;
    color.g = (float)((hex_color >> 8) & 0xFF) / 255.0f;
    color.b = (float)(hex_color & 0xFF) / 255.0f;
    return color;
}

RGBAVector4f convert_hex_color_to_rgbavec4f(uint32_t hex_color) {
    RGBAVector4f color;
    color.a = (float)((hex_color >> 24) & 0xFF) / 255.0f;
    color.r = (float)((hex_color >> 16) & 0xFF) / 255.0f;
    color.g = (float)((hex_color >> 8) & 0xFF) / 255.0f;
    color.b = (float)(hex_color & 0xFF) / 255.0f;
    return color;
}

void draw_pixel_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, RGBVector3f color) {
    uint32_t hex_color = convert_rgbvec3f_to_hex_color(color);
    int frame_buffer_position_x = coordinate_x;
    int frame_buffer_position_y = coordinate_y;
    if (frame_buffer_position_x < 0 || frame_buffer_position_x >= app_context->window_resolution.x || frame_buffer_position_y < 0 || frame_buffer_position_y >= app_context->window_resolution.y) {
        return; // Skip pixels that are outside the window boundaries
    }
    app_context->frame_buffer[frame_buffer_position_y * app_context->window_resolution.x + frame_buffer_position_x] = hex_color;
}

void draw_horizontal_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color) {
    if (x1 > x2) {
        int temp;
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp; // Swap coordinates to maintain the correct line direction
    }
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    //int line_direction_x = (delta_x >= 0) ? 1 : -1; // Determine the direction of the line in the x-axis
    int line_direction_y = (delta_y >= 0) ? 1 : -1; // Determine the direction of the line in the y-axis
    delta_y *= line_direction_y; // Make delta_y positive for the algorithm

    if (delta_x != 0) {
        int y = y1;
        int decision_parameter = 2 * delta_y - delta_x;
        for (int x = x1; x <= x2; x++) {
            draw_pixel_at_coordinates(app_context, x, y, color);
            if (decision_parameter >= 0) {
                y += line_direction_y;
                decision_parameter = decision_parameter - 2 * delta_x;
            }
            decision_parameter = decision_parameter + 2 * delta_y;
        }
    }
}

void draw_vertical_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color) {
    if (y1 > y2) {
        int temp;
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp; // Swap coordinates to maintain the correct line direction
    }
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int line_direction_x = (delta_x >= 0) ? 1 : -1; // Determine the direction of the line in the x-axis
    delta_x *= line_direction_x; // Make delta_x positive for the algorithm

    if (delta_y != 0) {
        int x = x1;
        int decision_parameter = 2 * delta_x - delta_y;
        for (int y = y1; y <= y2; y++) {
            draw_pixel_at_coordinates(app_context, x, y, color);
            if (decision_parameter >= 0) {
                x += line_direction_x;
                decision_parameter = decision_parameter - 2 * delta_y;
            }
            decision_parameter = decision_parameter + 2 * delta_x;
        }
    }
}

void draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color) {
    if (abs(x2 - x1) > abs(y2 - y1)) {
        draw_horizontal_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    } else {
        draw_vertical_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    }
}
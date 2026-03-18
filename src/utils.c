#include "../include/utils.h"

double get_delta_time(Uint64 *ticks_now, Uint64 *ticks_last) {
    *ticks_last = *ticks_now;
    *ticks_now = SDL_GetPerformanceCounter();
    return ((*ticks_now - *ticks_last) / (double)SDL_GetPerformanceFrequency() );
}

double get_instantaneous_frame_rate(Uint64 *ticks_now, Uint64 *ticks_last) {
    double delta_time = get_delta_time(ticks_now, ticks_last);
    return 1.0 / delta_time;
}

uint32_t convert_rgbvec3f_to_hex_color(RGBVector3f color) {
    uint32_t r = (uint32_t)(color.r * 255.0f) & 0xFF;
    uint32_t g = (uint32_t)(color.g * 255.0f) & 0xFF;
    uint32_t b = (uint32_t)(color.b * 255.0f) & 0xFF;
    return (r << 16) | (g << 8) | b;
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
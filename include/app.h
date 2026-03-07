#pragma once

#include "types.h"
#include <SDL2/SDL.h>

/**
 * @brief A structure representing the application context, including window, renderer, texture, and frame buffer information.
 * 
 * 57 bytes
 */
typedef struct {
    Vector2i window_resolution; // 8-bytes
    SDL_Window *window; // 8-bytes
    SDL_Renderer *renderer; // 8-bytes
    SDL_Texture *texture; // 8-bytes
    uint32_t *frame_buffer; // 8-bytes
    DepthBuffer *depth_buffer; // 8 bytes
    double time_accumulator; // 8-bytes
    double delta_time; // 8-bytes
    bool application_running; // 1-byte
} AppContext;

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title);
int cleanup_sdl_components(AppContext *app_context);
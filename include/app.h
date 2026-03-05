#pragma once

#include "types.h"
#include <SDL2/SDL.h>

/**
 * @brief A structure representing the application context, including window, renderer, texture, and frame buffer information.
 * 
 * 41 bytes
 */
typedef struct {
    /** The resolution of the application window. */
    Vector2i window_resolution; // 8-bytes
    /** The SDL window associated with the application. */
    SDL_Window *window; // 8-bytes
    /** The SDL renderer associated with the application. */
    SDL_Renderer *renderer; // 8-bytes
    /** The SDL texture used for rendering. */
    SDL_Texture *texture; // 8-bytes
    /** The frame buffer used for rendering. */
    uint32_t *frame_buffer; // 8-bytes
    /** A flag indicating whether the application is running. */
    bool application_running; // 1-byte
} AppContext;

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title);
int cleanup_sdl_components(AppContext *app_context);
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#pragma once

// Double check alignment!

typedef struct {
    int x;
    int y;
} Vector2i;

typedef struct {
    Vector2i top_left;
    Vector2i top_right;
    Vector2i bottom_left;
    Vector2i bottom_right;
    Vector2i top_middle;
    Vector2i bottom_middle;
    Vector2i absolute_middle;
} ScreenPositions;

typedef struct {
    Vector2i window_resolution; // 8-bytes
    SDL_Window *window; // 8-bytes
    SDL_Renderer *renderer; // 8-bytes
    SDL_Texture *texture; // 8-bytes
    uint32_t *frame_buffer; // 8-bytes
    bool application_running; // 1-byte
} AppContext;
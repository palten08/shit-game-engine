#pragma once

#include <SDL2/SDL.h>
#include "types.h"
#include "multithreading.h"
#include "rasterizer.h"

typedef struct ThreadPool ThreadPool;

typedef struct RasterizerJob RasterizerJob;

/**
 * @brief A structure representing the application context, including window, renderer, texture, and frame buffer information.
 * 
 * 57 bytes
 */
typedef struct AppContext {
    InputActionMap input_action_map; // 8960 bytes
    ThreadPool thread_pool; // 128 bytes
    RenderTriangle ***tile_triangles; // 8 bytes
    RasterizerJob *rasterizer_job_pool; // 8 bytes
    int *tile_counts; // 8 bytes
    int *tile_caps; // 8 bytes
    Vector2i window_resolution; // 8-bytes
    SDL_Window *window; // 8-bytes
    SDL_Renderer *renderer; // 8-bytes
    SDL_Texture *texture; // 8-bytes
    uint32_t *frame_buffer; // 8-bytes
    DepthBuffer *depth_buffer; // 8 bytes
    double time_accumulator; // 8-bytes
    double delta_time; // 8-bytes
    int tile_size; // 4 bytes
    int total_tiles; //4 bytes
    int tiles_x; // 4 bytes
    int tiles_y; // 4 bytes
    float scroll_wheel_delta_this_frame; // 4-bytes
    bool application_running; // 1-byte
} AppContext;

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title);
int cleanup_sdl_components(AppContext *app_context);
void handle_sdl_events(AppContext *app_context);
void create_app_thread_pool(AppContext *app_context, int thread_count);
#pragma once

#include "types.h"
#include "logging.h"

typedef struct {
    Vector2i window_resolution;
    const char *window_title; // 8 bytes
    const char *gif_path; // 8 bytes
    const char *log_path; // 8 bytes
    int resolution_x; // 4 bytes
    int resolution_y; // 4 bytes
    int rasterizer_tile_size; // 4 bytes
    LogVerbosity log_verbosity; // 4 bytes
    LogMode log_mode; // 4 bytes
    bool record_gif; // 1 byte
} EngineRunConfig;

void engine_init(AppContext *app_context, EngineRunConfig *config);
void engine_load_scene(Scene *scene, const char *scene_file_path);
void engine_frame_start(AppContext *app_context);
void engine_run(AppContext *app_context, Scene *loaded_scene);
void engine_frame_end(AppContext *app_context);
void engine_shutdown(AppContext *app_context, Scene *loaded_scene);
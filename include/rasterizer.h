#pragma once

#include "types.h"
#include "app.h"
#include "scene.h"

typedef struct RasterizerJob {
    AppContext *app_context; // 8 bytes
    RenderTriangle **render_triangles; // 8 bytes
    Scene *scene; // 8 bytes
    int tile_x; // 4 bytes
    int tile_y; // 4 bytes
    int tile_width; // 4 bytes
    int tile_height; // 4 bytes
    int triangle_count; // 4 bytes
} RasterizerJob;

int set_frame_buffer(AppContext *app_context, RGBVector3f color);

int clear_frame_buffer(AppContext *app_context);

void clear_and_lock_frame(AppContext *app_context);

int render_frame(AppContext *app_context, RenderList *render_list, Scene *scene);

void initialize_rasterizer_job_pool(AppContext *app_context, int tile_size);

void present_frame(AppContext *app_context);

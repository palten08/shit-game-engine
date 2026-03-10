#pragma once

#include "types.h"
#include "clipping.h"
#include "matrix_operations.h"
#include "virtual_camera.h"
#include "app.h"
#include "scene.h"

typedef struct {
    Scene *scene; // 8 bytes
    AppContext *app_context; // 8 bytes
    RenderTriangle *render_triangles; // 8 bytes
    int entity_start; // 4 bytes
    int entity_end; // 4 bytes
    int triangle_count; // 4 bytes
    int triangle_capacity; // 4 bytes
} RenderingPipelineJob;

void rendering_pipeline_worker(void *arg);
RenderList generate_render_list(Scene *scene, AppContext *app_context);
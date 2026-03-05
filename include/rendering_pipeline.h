#pragma once

#include "types.h"
#include "clipping.h"
#include "matrix_operations.h"
#include "virtual_camera.h"
#include "app.h"
#include "scene.h"

RenderList generate_render_list(Scene *scene, VirtualCamera *camera, AppContext *app_context);
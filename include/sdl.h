#pragma once

#include "types.h"
#include <SDL2/SDL.h>

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title);
int cleanup_sdl_components(AppContext *app_context);
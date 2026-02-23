#pragma once

#include "../include/types.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int set_frame_buffer(AppContext *app_context, uint32_t color);

int clear_frame_buffer(AppContext *app_context);

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title);

int cleanup_sdl_components(AppContext *app_context);
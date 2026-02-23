#include "../include/render.h"

int set_frame_buffer(AppContext *app_context, uint32_t color) {
    int texture_lock_result = SDL_LockTexture(app_context->texture, NULL, (void**)&app_context->frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
        return 1;
    }
    for (int y = 0; y < app_context->window_resolution.y; y++) {
        for (int x = 0; x < app_context->window_resolution.x; x++) {
            app_context->frame_buffer[y * app_context->window_resolution.x + x] = color;
        }
    }
    SDL_UnlockTexture(app_context->texture);
    SDL_RenderCopy(app_context->renderer, app_context->texture, NULL, NULL);
    SDL_RenderPresent(app_context->renderer);
    return 0;
}

int clear_frame_buffer(AppContext *app_context) {
    return set_frame_buffer(app_context, 0xFF000000); // Clear to black
}

int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title) {
    int sdl_init_result = SDL_Init(SDL_INIT_EVERYTHING);

    if (sdl_init_result != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    app_context->window_resolution = window_resolution;

    app_context->window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, app_context->window_resolution.x, app_context->window_resolution.y, SDL_WINDOW_SHOWN);
    if (app_context->window == NULL) {
        fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    app_context->renderer = SDL_CreateRenderer(app_context->window, -1, 0);
    if (app_context->renderer == NULL) {
        fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(app_context->window);
        SDL_Quit();
        return 1;
    }

    app_context->texture = SDL_CreateTexture(app_context->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, app_context->window_resolution.x, app_context->window_resolution.y);
    if (app_context->texture == NULL) {
        fprintf(stderr, "Error creating SDL texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(app_context->renderer);
        SDL_DestroyWindow(app_context->window);
        SDL_Quit();
        return 1;
    }

    app_context->application_running = true;

    app_context->frame_buffer = NULL;

    return 0;
}

int cleanup_sdl_components(AppContext *app_context) {
    if (app_context->texture) {
        SDL_DestroyTexture(app_context->texture);
    }
    if (app_context->renderer) {
        SDL_DestroyRenderer(app_context->renderer);
    }
    if (app_context->window) {
        SDL_DestroyWindow(app_context->window);
    }
    SDL_Quit();
    return 0;
}
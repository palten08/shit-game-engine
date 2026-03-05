#include "../include/app.h"
#include "../include/types.h"
#include <SDL2/SDL.h>

/**
 * @brief Initializes SDL components, including the window, renderer, and texture.
 * @param app_context A pointer to the application context to be initialized.
 * @param window_resolution The resolution of the application window.
 * @param window_title The title of the application window.
 * @return 0 on success, 1 on failure.
 */
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

/**
 * @brief Cleans up SDL components, including the window, renderer, and texture.
 * @param app_context A pointer to the application context to be cleaned up.
 * @return 0 on success.
 */
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
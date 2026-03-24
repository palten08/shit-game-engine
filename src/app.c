#include <SDL3/SDL.h>

#define MSF_GIF_IMPL

#include "../include/app.h"
#include "../include/types.h"
#include "../include/logging.h"

#ifndef VERSION_SHA
#define VERSION_SHA "dev"
#endif

/**
 * @brief Initializes SDL components, including the window, renderer, and texture.
 * @param app_context A pointer to the application context to be initialized.
 * @param window_resolution The resolution of the application window.
 * @param window_title The title of the application window.
 * @return 0 on success, 1 on failure.
 */
int initialize_sdl_components(AppContext *app_context, Vector2i window_resolution, const char *window_title) {
    bool sdl_metadata_initialized = SDL_SetAppMetadata(window_title, VERSION_SHA, "zone.pete.sge");
    if (!sdl_metadata_initialized) {
        LOG_ERROR("Error setting SDL app metadata: %s", SDL_GetError());
        return 1;
    }

    LOG_INFO("Initializing SDL3 sub-systems");
    LOG_INFO("Initializing SDL3 video subsystem");
    bool sdl_video_init_result = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (!sdl_video_init_result) {
        LOG_ERROR("Error initializing SDL video subsystem: %s", SDL_GetError());
        return 1;
    }
    LOG_INFO("Initializing SDL3 events subsystem");
    bool sdl_events_init_result = SDL_InitSubSystem(SDL_INIT_EVENTS);
    if (!sdl_events_init_result) {
        LOG_ERROR("Error initializing SDL events subsystem: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    LOG_DEBUG("Setting window resolution");
    app_context->window_resolution = window_resolution;

    LOG_INFO("Creating SDL window");
    app_context->window = SDL_CreateWindow(window_title, app_context->window_resolution.x, app_context->window_resolution.y, 0);
    if (app_context->window == NULL) {
        LOG_ERROR("Error creating SDL window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    LOG_INFO("Creating SDL renderer");
    app_context->renderer = SDL_CreateRenderer(app_context->window, NULL);
    if (app_context->renderer == NULL) {
        LOG_ERROR("Error creating SDL renderer: %s", SDL_GetError());
        SDL_DestroyWindow(app_context->window);
        SDL_Quit();
        return 1;
    }

    LOG_INFO("Enabling SDL vsync");
    bool vsync_result = SDL_SetRenderVSync(app_context->renderer, true);
    if (!vsync_result) {
        LOG_ERROR("Error enabling SDL vsync: %s", SDL_GetError());
        SDL_DestroyRenderer(app_context->renderer);
        SDL_DestroyWindow(app_context->window);
        SDL_Quit();
        return 1;
    }

    LOG_INFO("Creating SDL texture");
    app_context->texture = SDL_CreateTexture(app_context->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, app_context->window_resolution.x, app_context->window_resolution.y);
    if (app_context->texture == NULL) {
        LOG_ERROR("Error creating SDL texture: %s", SDL_GetError());
        SDL_DestroyRenderer(app_context->renderer);
        SDL_DestroyWindow(app_context->window);
        SDL_Quit();
        return 1;
    }

    app_context->application_running = true;

    app_context->frame_buffer = NULL;

    LOG_DEBUG("Finish SDL init");
    return 0;
}

/**
 * @brief Cleans up SDL components, including the window, renderer, and texture.
 * @param app_context A pointer to the application context to be cleaned up.
 * @return 0 on success.
 */
int cleanup_sdl_components(AppContext *app_context) {
    LOG_INFO("Cleaning up SDL components");
    LOG_INFO("Cleaning up SDL texture");
    if (app_context->texture) {
        SDL_DestroyTexture(app_context->texture);
    }
    LOG_INFO("Cleaning up SDL renderer");
    if (app_context->renderer) {
        SDL_DestroyRenderer(app_context->renderer);
    }
    LOG_INFO("Cleaning up SDL window");
    if (app_context->window) {
        SDL_DestroyWindow(app_context->window);
    }
    LOG_INFO("Quitting individual subsystems");
    LOG_INFO("Quitting SDL video subsystem");
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    LOG_INFO("Quitting SDL events subsystem");
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    LOG_DEBUG("Calling SDL quit");
    SDL_Quit();
    return 0;
}

void handle_sdl_events(AppContext *app_context) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_EVENT_QUIT:
                LOG_INFO("Handling SDL_EVENT_QUIT event");
                app_context->application_running = false;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                LOG_DEBUG("Handling SDL_EVENT_MOUSE_WHEEL event");
                app_context->scroll_wheel_delta_this_frame += sdl_event.wheel.y;
                break;
        }
    }
}

void create_app_thread_pool(AppContext *app_context, int thread_count) {
    LOG_INFO("Initializing thread pool with %d threads", thread_count);
    initialize_thread_pool(&app_context->thread_pool, thread_count);
}
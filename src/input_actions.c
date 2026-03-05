#include <SDL2/SDL.h>
#include "../include/input_actions.h"
#include "../include/app.h"

/**
 * @brief Creates an SDL window separate from the main window
 */
static SDL_Window *create_new_sub_window(char *title, int width, int height) {
        return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
}

static void handle_window_closure(AppContext *app_context, SDL_Event sdl_event) {
    SDL_Window *window_to_close = SDL_GetWindowFromID(sdl_event.window.windowID);
    if (window_to_close != NULL) {
        if (window_to_close == app_context->window) {
            app_context->application_running = false;
            return;
        }
        SDL_DestroyWindow(window_to_close);
    }
}

static void handle_application_quit(AppContext *app_context) {
    app_context->application_running = false;
}

/**
 * @brief Dispatcher function to listen for all SDL events and handle the relevant ones
 */
void handle_input(AppContext *app_context) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_QUIT:
                handle_application_quit(app_context);
                break;
            case SDL_KEYDOWN:
                break;
            case SDL_WINDOWEVENT:
                switch (sdl_event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        handle_window_closure(app_context, sdl_event);
                        break;
                }
                break;
        }
    }
}

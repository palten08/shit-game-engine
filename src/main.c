#include "../include/types.h"
#include "../include/render.h"
#include "../include/utils.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
int put_square(uint32_t *frame_buffer, int coordinate_x, int coordinate_y, int size, uint32_t color, SDL_Texture *texture, SDL_Renderer *renderer) {
    int texture_lock_result = SDL_LockTexture(texture, NULL, (void**)&frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
        return 1;
    }
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            frame_buffer[(coordinate_y + y) * WINDOW_RESOLUTION.x + (coordinate_x + x)] = color;
        }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    return 0;
}
*/

int main(void) {
    const char *WINDOW_TITLE = "Shit Game Engine";
    const Vector2i WINDOW_RESOLUTION = {800, 600};

    // https://wiki.libsdl.org/SDL2/SDL_Init
    int sdl_init_result = SDL_Init(SDL_INIT_EVERYTHING);

    if (sdl_init_result != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    AppContext app_context;

    if (initialize_sdl_components(&app_context, WINDOW_RESOLUTION, WINDOW_TITLE) != 0) {
        fprintf(stderr, "Error initializing SDL components\n");
        return 1;
    }

    Uint64 ticks_now = SDL_GetPerformanceCounter();
    Uint64 ticks_last = 0;
    double delta_time = 0;

    // Make the window black to start
    clear_frame_buffer(&app_context);

    while (app_context.application_running) {
        delta_time = get_delta_time(&ticks_now, &ticks_last);

        printf("Delta time is %f ms\n", delta_time);

        //put_square(frame_buffer, middle_x - 10, middle_y - 10, 20, 0xFFFF0000, new_sdl_texture, new_sdl_renderer);
        //put_pixel(frame_buffer, middle_x, middle_y, color, new_sdl_texture, new_sdl_renderer);

        // https://wiki.libsdl.org/SDL2/SDL_Event
        SDL_Event sdl_event;

        // https://wiki.libsdl.org/SDL2/SDL_PollEvent
        while (SDL_PollEvent(&sdl_event)) { 
            switch (sdl_event.type) {
                // https://wiki.libsdl.org/SDL2/SDL_QUIT ~ https://wiki.libsdl.org/SDL2/SDL_EventType
                case SDL_QUIT:
                    app_context.application_running = false;
                    break;
                // https://wiki.libsdl.org/SDL2/SDL_KEYDOWN ~ https://wiki.libsdl.org/SDL2/SDL_EventType
                case SDL_KEYDOWN:
                    printf("Key pressed\n");
                    printf("%d\n", sdl_event.key.keysym.sym);
                    switch (sdl_event.key.keysym.sym) {
                        // https://wiki.libsdl.org/SDL2/SDL_Keycode
                        // Debug thing to test out opening new windows
                        case SDLK_TAB:
                            SDL_Window *new_sub_sdl_window = SDL_CreateWindow("Sub Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_SHOWN);
                            break;
                    }
                    break;
                // https://wiki.libsdl.org/SDL2/SDL_WINDOWEVENT ~ https://wiki.libsdl.org/SDL2/SDL_EventType
                case SDL_WINDOWEVENT:
                    switch (sdl_event.window.event) {
                        // https://wiki.libsdl.org/SDL2/SDL_WindowEventID
                        // Debug thing to test out closing windows
                        case SDL_WINDOWEVENT_CLOSE:
                            printf("Window closed\n");
                            SDL_Window *window_to_close = SDL_GetWindowFromID(sdl_event.window.windowID);
                            SDL_DestroyWindow(window_to_close);
                            break;
                    }
                    break;
            }
        }
        clear_frame_buffer(&app_context); // Clear the screen to black each frame
    }

    cleanup_sdl_components(&app_context);
    return 0;
}

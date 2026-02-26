#include "../include/types.h"
#include "../include/frame_buffer.h"
#include "../include/sdl.h"
#include "../include/utils.h"
#include "../include/debug.h"
#include "../include/scene.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void) {
    const char *WINDOW_TITLE = "Shit Game Engine";
    const Vector2i WINDOW_RESOLUTION = {800, 600};

    AppContext app_context;

    if (initialize_sdl_components(&app_context, WINDOW_RESOLUTION, WINDOW_TITLE) != 0) {
        fprintf(stderr, "Error initializing SDL components\n");
        return 1;
    }

    Uint64 ticks_now = SDL_GetPerformanceCounter();
    Uint64 ticks_last = 0;
    double delta_time = 0;
    double time_accumulator = 0;

    ScreenPositions_Unioned screen_positions;

    screen_positions.named.top_left = (Vector2i){0, 0};
    screen_positions.named.top_middle = (Vector2i){WINDOW_RESOLUTION.x / 2, 0};
    screen_positions.named.top_right = (Vector2i){WINDOW_RESOLUTION.x, 0};
    screen_positions.named.middle_left = (Vector2i){0, WINDOW_RESOLUTION.y / 2};
    screen_positions.named.middle_right = (Vector2i){WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y / 2};
    screen_positions.named.bottom_left = (Vector2i){0, WINDOW_RESOLUTION.y};
    screen_positions.named.bottom_middle = (Vector2i){WINDOW_RESOLUTION.x / 2, WINDOW_RESOLUTION.y};
    screen_positions.named.bottom_right = (Vector2i){WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y};
    screen_positions.named.absolute_middle = (Vector2i){WINDOW_RESOLUTION.x / 2, WINDOW_RESOLUTION.y / 2};

    Scene test_scene = create_test_scene(&screen_positions);
    
    while (app_context.application_running) {
        delta_time = get_delta_time(&ticks_now, &ticks_last);

        double frame_rate = get_instantaneous_frame_rate(&ticks_now, &ticks_last);

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
                            if (window_to_close != NULL) {
                                if (window_to_close == app_context.window) {
                                    app_context.application_running = false;
                                    break;
                                }
                                SDL_DestroyWindow(window_to_close);
                            }
                            break;
                    }
                    break;
            }
        }
        uint32_t square_color;

        time_accumulator += delta_time;
        if (time_accumulator > 16.67) {
            square_color = SDL_GetPerformanceCounter() | 0xFF000000; // Get a color that changes every frame, but is always fully opaque
            time_accumulator = 0;
        }

        int texture_lock_result = SDL_LockTexture(app_context.texture, NULL, (void**)&app_context.frame_buffer, &(int){0});
        if (texture_lock_result != 0) {
            fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
            return 1;
        }

        clear_frame_buffer(&app_context); // Clear first

        write_scene_to_frame_buffer(&app_context, &test_scene);

        SDL_UnlockTexture(app_context.texture);
        SDL_RenderCopy(app_context.renderer, app_context.texture, NULL, NULL);
        SDL_RenderPresent(app_context.renderer); // Changes to the buffer are finished, now we can present them to the screen
    }

    cleanup_sdl_components(&app_context);
    return 0;
}

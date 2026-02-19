#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void) {
    const char *window_title = "Shit Game Engine";
    int resolution_x = 800;
    int resolution_y = 600;
    bool application_running = true;

    // Tracking time between frames
    // Uint64 goes from 0 to 18,446,744,073,709,551,615
    Uint64 time_now = SDL_GetPerformanceCounter();
    Uint64 time_last = 0;
    double delta_time = 0;

    // https://wiki.libsdl.org/SDL2/SDL_Init
    int sdl_init_result = SDL_Init(SDL_INIT_EVERYTHING);

    if (sdl_init_result != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // https://wiki.libsdl.org/SDL2/SDL_CreateWindow
    SDL_Window *new_sdl_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_x, resolution_y, SDL_WINDOW_SHOWN);

    if (new_sdl_window == NULL) {
        fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *new_sdl_renderer = SDL_CreateRenderer(new_sdl_window, -1, 0);

    if (new_sdl_renderer == NULL) {
        fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(new_sdl_window);
        SDL_Quit();
        return 1;
    }

    while (application_running) {
        // I think everything is going to happen in here

        // Update delta time
        time_last = time_now;
        time_now = SDL_GetPerformanceCounter();
        // time_now minus time_last — how many ticks elapsed this frame
        // SDL_GetPerformanceFrequency() — how many ticks happen per second on this machine
        // Dividing ticks by ticks-per-second gives you seconds
        // Multiplying by 1000 converts seconds to milliseconds
        delta_time = ((time_now - time_last)*1000 / (double)SDL_GetPerformanceFrequency() );

        // https://wiki.libsdl.org/SDL2/SDL_Event
        SDL_Event sdl_event;

        // https://wiki.libsdl.org/SDL2/SDL_PollEvent
        while (SDL_PollEvent(&sdl_event)) { 
            switch (sdl_event.type) {
                // https://wiki.libsdl.org/SDL2/SDL_QUIT ~ https://wiki.libsdl.org/SDL2/SDL_EventType
                case SDL_QUIT:
                    application_running = false;
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
    }

    SDL_DestroyRenderer(new_sdl_renderer);
    SDL_DestroyWindow(new_sdl_window);
    SDL_Quit();
    return 0;
}
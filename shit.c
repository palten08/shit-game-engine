#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void) {
    const char *window_title = "Shit Game Engine";
    int resolution_x = 800;
    int resolution_y = 600;
    bool application_running = true;

    // https://wiki.libsdl.org/SDL2/SDL_Init
    SDL_Init(SDL_INIT_EVERYTHING);

    // https://wiki.libsdl.org/SDL2/SDL_CreateWindow
    SDL_Window *new_sdl_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_x, resolution_y, SDL_WINDOW_SHOWN);

    while (application_running) {
        // I think everything is going to happen in here

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

    SDL_DestroyWindow(new_sdl_window); // Probably could just call quit but maybe it's better to be diligent about freeing up the stuff you made to avoid memory leaks and whatever
    SDL_Quit();
    return 0;
}
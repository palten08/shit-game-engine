#include "../include/types.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

Vector2i window_resolution = {800, 600};

int put_square(uint32_t *frame_buffer, int coordinate_x, int coordinate_y, int size, uint32_t color, SDL_Texture *texture, SDL_Renderer *renderer) {
    int texture_lock_result = SDL_LockTexture(texture, NULL, (void**)&frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
        return 1;
    }
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            frame_buffer[(coordinate_y + y) * window_resolution.x + (coordinate_x + x)] = color;
        }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    return 0;
}

int clear_buffer(uint32_t *frame_buffer, uint32_t color, SDL_Texture *texture, SDL_Renderer *renderer) {
    int texture_lock_result = SDL_LockTexture(texture, NULL, (void**)&frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
        return 1;
    }
    for (int y = 0; y < window_resolution.y; y++) {
        for (int x = 0; x < window_resolution.x; x++) {
            frame_buffer[y * window_resolution.x + x] = color;
        }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    return 0;
}

int main(void) {
    const char *window_title = "Shit Game Engine";
    bool application_running = true;

    // Tracking time between frames
    // Uint64 goes from 0 to 18,446,744,073,709,551,615
    Uint64 ticks_now = SDL_GetPerformanceCounter();
    Uint64 ticks_last = 0;
    double delta_time = 0;

    // Frame buffer?
    //uint32_t *frame_buffer = (uint32_t *)malloc(window_resolution.x * window_resolution.y * sizeof(uint32_t));
    uint32_t *frame_buffer = NULL;

    // https://wiki.libsdl.org/SDL2/SDL_Init
    int sdl_init_result = SDL_Init(SDL_INIT_EVERYTHING);

    if (sdl_init_result != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // https://wiki.libsdl.org/SDL2/SDL_CreateWindow
    SDL_Window *new_sdl_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_resolution.x, window_resolution.y, SDL_WINDOW_SHOWN);

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

    SDL_Texture *new_sdl_texture = SDL_CreateTexture(new_sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_resolution.x, window_resolution.y);

    if (new_sdl_texture == NULL) {
        fprintf(stderr, "Error creating SDL texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(new_sdl_renderer);
        SDL_DestroyWindow(new_sdl_window);
        SDL_Quit();
        return 1;
    }

    // Attempt to make the entire window white

    // Lock the texture first
    int texture_lock_result = SDL_LockTexture(new_sdl_texture, NULL, (void**)&frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(new_sdl_texture);
        SDL_DestroyRenderer(new_sdl_renderer);
        SDL_DestroyWindow(new_sdl_window);
        SDL_Quit();
        return 1;
    }

    // Push initial pixels into frame buffer?
    for (int y = 0; y < window_resolution.y; y++) {
        for (int x = 0; x < window_resolution.x; x++) {
            frame_buffer[y * window_resolution.x + x] = 0xFFFFFFFF; // ARGB format, opaque white
        }
    }

    // Unlock the texture after modifying the frame buffer
    SDL_UnlockTexture(new_sdl_texture); // Apparently this can't fail so it doesn't return an int

    // Copy the texture to the renderer
    int texture_copy_result = SDL_RenderCopy(new_sdl_renderer, new_sdl_texture, NULL, NULL);

    // Render?
    SDL_RenderPresent(new_sdl_renderer);

    while (application_running) {
        // I think everything is going to happen in here

        // Update delta time
        ticks_last = ticks_now;
        ticks_now = SDL_GetPerformanceCounter();
        // ticks_now minus ticks_last — how many ticks elapsed this frame
        // SDL_GetPerformanceFrequency() — how many ticks happen per second on this machine
        // Dividing ticks by ticks-per-second gives you seconds
        // Multiplying by 1000 converts seconds to milliseconds
        delta_time = ((ticks_now - ticks_last)*1000 / (double)SDL_GetPerformanceFrequency() );


        //put_square(frame_buffer, middle_x - 10, middle_y - 10, 20, 0xFFFF0000, new_sdl_texture, new_sdl_renderer);
        //put_pixel(frame_buffer, middle_x, middle_y, color, new_sdl_texture, new_sdl_renderer);

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
        clear_buffer(frame_buffer, 0xFF000000, new_sdl_texture, new_sdl_renderer); // Clear the screen to black each frame
    }

    SDL_DestroyTexture(new_sdl_texture);
    SDL_DestroyRenderer(new_sdl_renderer);
    SDL_DestroyWindow(new_sdl_window);
    SDL_Quit();
    return 0;
}

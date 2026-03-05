#include "../include/types.h"
#include "../include/rasterizer.h"
#include "../include/app.h"
#include "../include/utils.h"
#include "../include/debug.h"
#include "../include/scene.h"
#include "../include/virtual_camera.h"
#include "../include/input_actions.h"
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

    VirtualCamera virtual_camera = initialize_virtual_camera(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y, 90.0f, 0.1f, 500.0f, (Vector3f){0.0f, 0.0f, 250.0f});

    //Scene test_scene = create_test_scene();
    Scene test_scene = create_scene_from_file("/home/pete/C Development/sge-test-project/main_scene.json");
    
    while (app_context.application_running) {
        delta_time = get_delta_time(&ticks_now, &ticks_last);

        double frame_rate = get_instantaneous_frame_rate(&ticks_now, &ticks_last);

        handle_input(&app_context);

        // To-do move this elsewhere later
        time_accumulator += delta_time;
        if (time_accumulator > 0.0167 ) {
            time_accumulator = 0;
        }

        // All of this stuff can likely go to its own thing but that's for later
        int texture_lock_result = SDL_LockTexture(app_context.texture, NULL, (void**)&app_context.frame_buffer, &(int){0});
        if (texture_lock_result != 0) {
            fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
            return 1;
        }

        clear_frame_buffer(&app_context); // Clear first

        //test_scene = test_update_scene(&test_scene, &virtual_camera, delta_time);
        write_scene_to_frame_buffer(&app_context, &test_scene);

        SDL_UnlockTexture(app_context.texture);
        SDL_RenderCopy(app_context.renderer, app_context.texture, NULL, NULL);
        SDL_RenderPresent(app_context.renderer); // Changes to the buffer are finished, now we can present them to the screen
    }

    cleanup_sdl_components(&app_context);
    return 0;
}

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include "../include/engine_core.h"
#include "scene.h"
#include "app.h"
#include "logging.h"
#include "rendering_pipeline.h"
#include "ecs.h"
#include "input_actions.h"
#include "utils.h"

void engine_init(AppContext *app_context, EngineRunConfig *config) {
    printf("Engine initialization start");

    if (config->log_path && config->log_path[0] != '\0') {
        initialize_engine_logging(config->log_verbosity, config->log_mode, config->log_path);
        printf("File logging at verbosity level %d initiated to path: %s\n", config->log_verbosity, config->log_path);
    } else {
        initialize_engine_logging(config->log_verbosity, config->log_mode, NULL);
        printf("Stdout logging at verbosity level %d initiated", config->log_verbosity);
    }

    LOG_INFO("Engine initialized with log path: %s", config->log_path ? config->log_path : "stdout");

    if (config->record_gif) {
        LOG_INFO("GIF recording enabled (Output path: %s)", config->gif_path);
        app_context->record_gif = true;
        GifRecordConfig gif_record_config = {0};
        gif_record_config.gif_path = config->gif_path;
        gif_record_config.gif_width = config->window_resolution.x;
        gif_record_config.gif_height = config->window_resolution.y;
        gif_record_config.centiseconds_per_frame = 10;
        gif_record_config.gif_quality = 16;
        LOG_INFO("GIF resolution: %d x %d", config->window_resolution.x, config->window_resolution.y);
        LOG_INFO("GIF quality: %d | Centiseconds per frame: %d", gif_record_config.gif_quality, gif_record_config.centiseconds_per_frame);
        app_context->gif_config = gif_record_config;
        app_context->gif_pixels = malloc(gif_record_config.gif_width * gif_record_config.gif_height * 4);
        app_context->gif_state = malloc(sizeof(MsfGifState));
        memset(app_context->gif_state, 0, sizeof(MsfGifState));
        msf_gif_begin(app_context->gif_state, gif_record_config.gif_width, gif_record_config.gif_height);
    }

    char window_title[128];
    if (app_context->record_gif) {
        snprintf(window_title, sizeof(window_title), "%s (Recording GIF)", config->window_title);
    } else {
        snprintf(window_title, sizeof(window_title), "%s", config->window_title);
    }

    if (initialize_sdl_components(app_context, config->window_resolution, window_title) != 0) {
        LOG_ERROR("Error initializing SDL components");
        exit(1);
    }

    app_context->depth_buffer = malloc(sizeof(DepthBuffer));
    if (!app_context->depth_buffer) {
        LOG_ERROR("Failed to allocate memory for depth buffer");
        exit(1);
    }

    app_context->depth_buffer->width = app_context->window_resolution.x;
    app_context->depth_buffer->height = app_context->window_resolution.y;
    app_context->depth_buffer->depth_values = malloc(app_context->window_resolution.x * app_context->window_resolution.y * sizeof(float));
    if (!app_context->depth_buffer->depth_values) {
        LOG_ERROR("Failed to allocate memory for depth buffer values");
        free(app_context->depth_buffer);
        exit(1);
    }

    int thread_count = sysconf(_SC_NPROCESSORS_ONLN) - 1;
    if (thread_count < 1) thread_count = 1;
    LOG_DEBUG("Initializing thread pool with %d threads", thread_count);
    create_app_thread_pool(app_context, thread_count);

    LOG_DEBUG("Initializing rasterizer job pool");
    initialize_rasterizer_job_pool(app_context, config->rasterizer_tile_size);

    LOG_INFO("Engine initialization finished");

    return;
}

void engine_load_scene(Scene *scene, const char *scene_file_path) {
    LOG_DEBUG("Begin engine scene loading");

    scene->entity_manager.next_id = 0;

    LOG_INFO("Registering engine default components");
    LOG_DEBUG("Registering transform component");
    TRANSFORM = register_component(scene, sizeof(TransformComponent), "transform", parse_transform_component);
    LOG_DEBUG("Registering mesh component");
    MESH = register_component(scene, sizeof(MeshComponent), "mesh", parse_mesh_component);

    LOG_INFO("Loading scene from file: %s", scene_file_path);
    load_scene_from_binary(scene_file_path, scene);
    //load_scene_from_file(scene, scene_file_path);

    return;
}

void engine_frame_start(AppContext *app_context) {
    Uint64 ticks_now = SDL_GetPerformanceCounter();
    app_context->delta_time = (double)(ticks_now - app_context->ticks_last) / SDL_GetPerformanceFrequency();
    app_context->ticks_last = ticks_now;

    update_input_actions(&app_context->input_action_map, app_context);

    handle_sdl_events(app_context);

    app_context->time_accumulator += app_context->delta_time;
    if (app_context->time_accumulator > 0.0167 ) {
        app_context->time_accumulator = 0;
    }

    for (int i = 0; i < app_context->window_resolution.x * app_context->window_resolution.y; i++) {
        app_context->depth_buffer->depth_values[i] = FLT_MAX; // Reset depth buffer values to maximum depth
    }
}

void engine_run(AppContext *app_context, Scene *loaded_scene) {
    app_context->systems_timer = SDL_GetPerformanceCounter();
    run_systems(loaded_scene, app_context);
    app_context->rendering_pipeline_timer = SDL_GetPerformanceCounter();
    app_context->render_list = generate_render_list(loaded_scene, app_context);
    app_context->rasterizer_timer = SDL_GetPerformanceCounter();
    render(app_context, &app_context->render_list, loaded_scene);
    app_context->update_end_timer = SDL_GetPerformanceCounter();
}

void engine_frame_end(AppContext *app_context) {
    double freq = SDL_GetPerformanceFrequency();
    static int frame = 0;
    if (frame++ % 60 == 0) {
        LOG_DEBUG("systems: %.2fms  pipeline: %.2fms  render: %.2fms  tris: %d\n", (app_context->rendering_pipeline_timer-app_context->systems_timer)/freq*1000, (app_context->rasterizer_timer-app_context->rendering_pipeline_timer)/freq*1000, (app_context->update_end_timer-app_context->rasterizer_timer)/freq*1000, app_context->render_list.triangle_count);
    }

    free(app_context->render_list.triangles);
    app_context->render_list.triangles = NULL;
    app_context->render_list.triangle_count = 0;
    if (app_context->record_gif) {
        SDL_RenderReadPixels(app_context->renderer, NULL, SDL_PIXELFORMAT_RGBA32, app_context->gif_pixels, app_context->gif_config.gif_width * 4);
        msf_gif_frame(app_context->gif_state, app_context->gif_pixels, app_context->gif_config.centiseconds_per_frame, app_context->gif_config.gif_quality, app_context->gif_config.gif_width * 4);
    }
}

void engine_shutdown(AppContext *app_context) {
    LOG_INFO("Attempting to cleanly shut the engine down");
    if (app_context->record_gif) {
        MsfGifResult result = msf_gif_end(app_context->gif_state);
        if (result.data) {
            FILE * fp = fopen(app_context->gif_config.gif_path, "wb");
            fwrite(result.data, result.dataSize, 1, fp);
            fclose(fp);
        }
        msf_gif_free(result);
        free(app_context->gif_pixels);
        free(app_context->gif_state);
    }
    destroy_thread_pool(&app_context->thread_pool);
    free(app_context->depth_buffer->depth_values);
    free(app_context->depth_buffer);
    cleanup_sdl_components(app_context);
    exit(0);
}





































/**
int engine_run(EngineRunConfig *config) {
    bool gif_recording = false;
    MsfGifState gifState = {0};
    uint8_t *gif_pixels;
    int gif_width = config->window_resolution.x, height = config->window_resolution.y, centisecondsPerFrame = 10, quality = 16;

    if (config->record_gif) {
        LOG_DEBUG("GIF recording enabled");
        gif_recording = true;
        msf_gif_begin(&gifState, gif_width, height);
        gif_pixels = malloc(gif_width * height * 4);
    }

    char window_title[128];
    if (gif_recording) {
        snprintf(window_title, sizeof(window_title), "%s (Recording GIF)", config->window_title);
    } else {
        snprintf(window_title, sizeof(window_title), "%s", config->window_title);
    }

    Scene loaded_scene = {0};
    loaded_scene.entity_manager.next_id = 0;

    AppContext app_context = {0};

    if (config->log_path && config->log_path[0] != '\0') {
        app_context.log_path = config->log_path;
        app_context.log_mode = LOG_MODE_FILE;
        app_context.log_verbosity = config->log_verbosity;
    } else {
        app_context.log_mode = LOG_MODE_STDOUT;
        app_context.log_verbosity = config->log_verbosity;   
    }

    if (initialize_sdl_components(&app_context, config->window_resolution, window_title) != 0) {
        LOG_ERROR("Error initializing SDL components");
        return 1;
    }

    app_context.depth_buffer = malloc(sizeof(DepthBuffer));
    if (!app_context.depth_buffer) {
        LOG_ERROR("Failed to allocate memory for depth buffer");
        return 1;
    }

    app_context.depth_buffer->width = app_context.window_resolution.x;
    app_context.depth_buffer->height = app_context.window_resolution.y;
    app_context.depth_buffer->depth_values = malloc(app_context.window_resolution.x * app_context.window_resolution.y * sizeof(float));
    if (!app_context.depth_buffer->depth_values) {
        LOG_ERROR("Failed to allocate memory for depth buffer values");
        free(app_context.depth_buffer);
        return 1;
    }

    int thread_count = sysconf(_SC_NPROCESSORS_ONLN) - 1;
    if (thread_count < 1) thread_count = 1;
    LOG_DEBUG("Initializing thread pool with %d threads", thread_count);
    create_app_thread_pool(&app_context, thread_count);

    LOG_DEBUG("Initializing rasterizer job pool");
    initialize_rasterizer_job_pool(&app_context, config->rasterizer_tile_size);

    Uint64 ticks_now = SDL_GetPerformanceCounter();
    Uint64 ticks_last = 0;
    app_context.delta_time = 0;
    app_context.time_accumulator = 0;

    // Call the function associated with the EngineRunConfig now?
    config->project_init_function(&loaded_scene, &app_context);

    config->scene_setup_function(&loaded_scene, &app_context);
    //load_scene_from_file(&loaded_scene, "scenes/main_scene.json");

    while (app_context.application_running) {
        app_context.delta_time = get_delta_time(&ticks_now, &ticks_last);

        update_input_actions(&app_context.input_action_map, &app_context);

        handle_sdl_events(&app_context);

        app_context.time_accumulator += app_context.delta_time;
        if (app_context.time_accumulator > 0.0167 ) {
            app_context.time_accumulator = 0;
        }

        for (int i = 0; i < app_context.window_resolution.x * app_context.window_resolution.y; i++) {
            app_context.depth_buffer->depth_values[i] = FLT_MAX; // Reset depth buffer values to maximum depth
        }

        Uint64 t0 = SDL_GetPerformanceCounter();
        run_systems(&loaded_scene, &app_context);
        Uint64 t1 = SDL_GetPerformanceCounter();
        RenderList render_list = generate_render_list(&loaded_scene, &app_context);
        Uint64 t2 = SDL_GetPerformanceCounter();
        render(&app_context, &render_list);
        Uint64 t3 = SDL_GetPerformanceCounter();

        double freq = SDL_GetPerformanceFrequency();
        static int frame = 0;
        if (frame++ % 60 == 0) {
            LOG_DEBUG("systems: %.2fms  pipeline: %.2fms  render: %.2fms  tris: %d\n", (t1-t0)/freq*1000, (t2-t1)/freq*1000, (t3-t2)/freq*1000, render_list.triangle_count);
        }

        free(render_list.triangles);
        if (gif_recording) {
            SDL_RenderReadPixels(app_context.renderer, NULL, SDL_PIXELFORMAT_RGBA32, gif_pixels, gif_width * 4);
            msf_gif_frame(&gifState, gif_pixels, centisecondsPerFrame, quality, gif_width * 4);
        }
    }

    if (gif_recording) {
        MsfGifResult result = msf_gif_end(&gifState);
        if (result.data) {
            FILE * fp = fopen(config->gif_path, "wb");
            fwrite(result.data, result.dataSize, 1, fp);
            fclose(fp);
        }
        msf_gif_free(result);
        free(gif_pixels);
    }

    destroy_thread_pool(&app_context.thread_pool);
    free(app_context.depth_buffer->depth_values);
    free(app_context.depth_buffer);
    cleanup_sdl_components(&app_context);
    return 0;
}
    */
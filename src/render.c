#include "../include/render.h"
#include "../include/types.h"

int set_frame_buffer(AppContext *app_context, uint32_t color) {
    for (int y = 0; y < app_context->window_resolution.y; y++) {
        for (int x = 0; x < app_context->window_resolution.x; x++) {
            app_context->frame_buffer[y * app_context->window_resolution.x + x] = color;
        }
    }
    return 0;
}

int clear_frame_buffer(AppContext *app_context) {
    return set_frame_buffer(app_context, 0xFF000000); // Clear to black
}

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

int put_square_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, int size, uint32_t color, PixelAlignment alignment) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int frame_buffer_position_x = coordinate_x + x;
            int frame_buffer_position_y = coordinate_y + y;
            switch (alignment) {
                case TOP_LEFT:
                    // No adjustment needed
                    break;
                case TOP_RIGHT:
                    frame_buffer_position_x -= size;
                    break;
                case BOTTOM_LEFT:
                    frame_buffer_position_y -= size;
                    break;
                case BOTTOM_RIGHT:
                    frame_buffer_position_x -= size;
                    frame_buffer_position_y -= size;
                    break;
                case TOP_MIDDLE:
                    frame_buffer_position_x -= size / 2;
                    break;
                case BOTTOM_MIDDLE:
                    frame_buffer_position_x -= size / 2;
                    frame_buffer_position_y -= size;
                    break;
                case MIDDLE_LEFT:
                    frame_buffer_position_y -= size / 2;
                    break;
                case MIDDLE_RIGHT:
                    frame_buffer_position_x -= size;
                    frame_buffer_position_y -= size / 2;
                    break;
                case ABSOLUTE_MIDDLE:
                    frame_buffer_position_x -= size / 2;
                    frame_buffer_position_y -= size / 2;
                    break;
            }
            if (frame_buffer_position_x < 0 || frame_buffer_position_x >= app_context->window_resolution.x || frame_buffer_position_y < 0 || frame_buffer_position_y >= app_context->window_resolution.y) {
                continue; // Skip pixels that are outside the window boundaries
            }
            app_context->frame_buffer[frame_buffer_position_y * app_context->window_resolution.x + frame_buffer_position_x] = color;
        }
    }
    return 0;
}

int draw_horizontal_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color) {
    if (x1 > x2) {
        int temp;
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp; // Swap coordinates to maintain the correct line direction
    }
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    //int line_direction_x = (delta_x >= 0) ? 1 : -1; // Determine the direction of the line in the x-axis
    int line_direction_y = (delta_y >= 0) ? 1 : -1; // Determine the direction of the line in the y-axis
    delta_y *= line_direction_y; // Make delta_y positive for the algorithm

    if (delta_x != 0) {
        int y = y1;
        int decision_parameter = 2 * delta_y - delta_x;
        for (int x = x1; x <= x2; x++) {
            put_square_at_coordinates(app_context, x, y, 1, color, TOP_LEFT);
            if (decision_parameter >= 0) {
                y += line_direction_y;
                decision_parameter = decision_parameter - 2 * delta_x;
            }
            decision_parameter = decision_parameter + 2 * delta_y;
        }
    }
    return 0;
}

int draw_vertical_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color) {
    if (y1 > y2) {
        int temp;
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp; // Swap coordinates to maintain the correct line direction
    }
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int line_direction_x = (delta_x >= 0) ? 1 : -1; // Determine the direction of the line in the x-axis
    delta_x *= line_direction_x; // Make delta_x positive for the algorithm

    if (delta_y != 0) {
        int x = x1;
        int decision_parameter = 2 * delta_x - delta_y;
        for (int y = y1; y <= y2; y++) {
            put_square_at_coordinates(app_context, x, y, 1, color, TOP_LEFT);
            if (decision_parameter >= 0) {
                x += line_direction_x;
                decision_parameter = decision_parameter - 2 * delta_y;
            }
            decision_parameter = decision_parameter + 2 * delta_x;
        }
    }
    return 0;
}

int draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color) {
    if (abs(x2 - x1) > abs(y2 - y1)) {
        return draw_horizontal_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    } else {
        return draw_vertical_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    }
}

int render_scene(AppContext *app_context, Scene *scene) {
    for (int i = 0; i < scene->square_count; i++) {
        SquareData square = scene->squares[i];
        put_square_at_coordinates(app_context, square.position.x, square.position.y, square.size, square.color, ABSOLUTE_MIDDLE);
    }
    for (int i = 0; i < scene->line_count; i++) {
        LineData line = scene->lines[i];
        draw_line_between_coordinates(app_context, line.start.x, line.start.y, line.end.x, line.end.y, line.color);
    }
    return 0;
}
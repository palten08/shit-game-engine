#include <SDL2/SDL.h>

#include "../include/rasterizer.h"
#include "../include/types.h"
#include "../include/app.h"
#include "../include/coordinates.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MAX((lo), MIN((x), (hi))))

/**
 * @brief Sets the entire frame buffer to a specific color.
 * @param app_context A pointer to the application context.
 * @param color The color to set the frame buffer to.
 * @return 0 on success.
 */
int set_frame_buffer(AppContext *app_context, uint32_t color) {
    for (int y = 0; y < app_context->window_resolution.y; y++) {
        for (int x = 0; x < app_context->window_resolution.x; x++) {
            app_context->frame_buffer[y * app_context->window_resolution.x + x] = color;
        }
    }
    return 0;
}

/**
 * @brief Clears the frame buffer by setting it to a default color (black).
 * @param app_context A pointer to the application context.
 * @return 0 on success.
 */
int clear_frame_buffer(AppContext *app_context) {
    return set_frame_buffer(app_context, 0xFF000000); // Clear to black
}

/**
 * @brief Draws a pixel at the specified coordinates with the given color.
 * The coordinates supplied are to be screen space
 * @param app_context A pointer to the application context.
 * @param coordinate_x The x-coordinate of the pixel to be drawn.
 * @param coordinate_y The y-coordinate of the pixel to be drawn.
 * @param color The color of the pixel to be drawn.
 * @return 0 on success, 1 if the coordinates are outside the window boundaries.
 */
int draw_pixel_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, uint32_t color) {
    int frame_buffer_position_x = coordinate_x;
    int frame_buffer_position_y = coordinate_y;
    if (frame_buffer_position_x < 0 || frame_buffer_position_x >= app_context->window_resolution.x || frame_buffer_position_y < 0 || frame_buffer_position_y >= app_context->window_resolution.y) {
        return 1; // Skip pixels that are outside the window boundaries
    }
    app_context->frame_buffer[frame_buffer_position_y * app_context->window_resolution.x + frame_buffer_position_x] = color;
    return 0;
}

/**
 * @brief Draws a line between two coordinates using Bresenham's line algorithm.
 * @param app_context A pointer to the application context.
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line.
 * @param color The color of the line to be drawn.
 * @return 0 on success.
 */
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
            draw_pixel_at_coordinates(app_context, x, y, color);
            if (decision_parameter >= 0) {
                y += line_direction_y;
                decision_parameter = decision_parameter - 2 * delta_x;
            }
            decision_parameter = decision_parameter + 2 * delta_y;
        }
    }
    return 0;
}

/**
 * @brief Draws a line between two coordinates using Bresenham's line algorithm.
 * @param app_context A pointer to the application context.
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line.
 * @param color The color of the line to be drawn.
 * @return 0 on success.
 */
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
            draw_pixel_at_coordinates(app_context, x, y, color);
            if (decision_parameter >= 0) {
                x += line_direction_x;
                decision_parameter = decision_parameter - 2 * delta_y;
            }
            decision_parameter = decision_parameter + 2 * delta_x;
        }
    }
    return 0;
}

/**
 * @brief Draws a line between two coordinates using Bresenham's line algorithm.
 * The absolute value of the slope of the line is used to determine whether to use the horizontal or vertical line drawing algorithm for better performance and accuracy.
 * @param app_context A pointer to the application context.
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line.
 * @param color The color of the line to be drawn.
 * @return 0 on success.
 */
int draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, uint32_t color) {
    if (abs(x2 - x1) > abs(y2 - y1)) {
        return draw_horizontal_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    } else {
        return draw_vertical_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    }
}

float edge_function(Vector2f a, Vector2f b, Vector2f c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void fill_triangle(RenderTriangle *triangle, AppContext *app_context) {
    // Triangle filling with barycentric coordinates


    Vector2f vertex0 = { (float)triangle->screen_positions[0].x, (float)triangle->screen_positions[0].y };
    Vector2f vertex1 = { (float)triangle->screen_positions[1].x, (float)triangle->screen_positions[1].y };
    Vector2f vertex2 = { (float)triangle->screen_positions[2].x, (float)triangle->screen_positions[2].y };

    float triangle_area = edge_function(vertex0, vertex1, vertex2);
    if (triangle_area >= 0) {
        return; // Skip degenerate triangles
    }

    // Compute the bounding box of the triangle
    int min_x = CLAMP(MIN(MIN(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), 0, app_context->window_resolution.x - 1);
    int max_x = CLAMP(MAX(MAX(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), 0, app_context->window_resolution.x - 1);
    int min_y = CLAMP(MIN(MIN(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), 0, app_context->window_resolution.y - 1);
    int max_y = CLAMP(MAX(MAX(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), 0, app_context->window_resolution.y - 1);

    // Step deltas
    float w0_step_x = vertex1.y - vertex2.y;
    float w0_step_y = vertex2.x - vertex1.x;
    float w1_step_x = vertex2.y - vertex0.y;
    float w1_step_y = vertex0.x - vertex2.x;
    float w2_step_x = vertex0.y - vertex1.y;
    float w2_step_y = vertex1.x - vertex0.x;

    Vector2f start_point = { (float)(min_x + 0.5f), (float)(min_y + 0.5f) }; // Start at the center of the top-left pixel in the bounding box
    float w0_row = edge_function(vertex1, vertex2, start_point);
    float w1_row = edge_function(vertex2, vertex0, start_point);
    float w2_row = edge_function(vertex0, vertex1, start_point);

    for (int y = min_y; y <= max_y; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;
        for (int x = min_x; x <= max_x; x++) {
            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) { // Check if the point is inside the triangle
                float bary0 = w0 / triangle_area;
                float bary1 = w1 / triangle_area;
                float bary2 = w2 / triangle_area;
                float depth = bary0 * triangle->depth_values[0] + bary1 * triangle->depth_values[1] + bary2 * triangle->depth_values[2]; // Interpolate depth using barycentric coordinates
                int index = y * app_context->window_resolution.x + x;
                if (depth < app_context->depth_buffer->depth_values[index]) { // Depth test
                    app_context->depth_buffer->depth_values[index] = depth; // Update depth buffer
                    draw_pixel_at_coordinates(app_context, x, y, triangle->color);
                }
            }
            w0 += w0_step_x;
            w1 += w1_step_x;
            w2 += w2_step_x;
        }
        w0_row += w0_step_y;
        w1_row += w1_step_y;
        w2_row += w2_step_y;
    }
}

/**
 * @brief Iterates over the render list and draws all the triangles contained within it.
 * @param app_context A pointer to the application context.
 * @param render_list A pointer to the render list to be rendered.
 * @return 0 on success.
 */
int rasterize_render_list(AppContext *app_context, RenderList *render_list) {
    for (int i = 0; i < render_list->triangle_count; i++) {
        RenderTriangle *triangle = &render_list->triangles[i];
        //draw_line_between_coordinates(app_context, triangle->screen_positions[0].x, triangle->screen_positions[0].y, triangle->screen_positions[1].x, triangle->screen_positions[1].y, triangle->color);
        //draw_line_between_coordinates(app_context, triangle->screen_positions[1].x, triangle->screen_positions[1].y, triangle->screen_positions[2].x, triangle->screen_positions[2].y, triangle->color);
        //draw_line_between_coordinates(app_context, triangle->screen_positions[2].x, triangle->screen_positions[2].y, triangle->screen_positions[0].x, triangle->screen_positions[0].y, triangle->color);
        fill_triangle(triangle, app_context);
    }
    return 0;
}

int render(AppContext *app_context, RenderList *render_list) {
    int texture_lock_result = SDL_LockTexture(app_context->texture, NULL, (void**)&app_context->frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        fprintf(stderr, "Error locking SDL texture: %s\n", SDL_GetError());
        return 1;
    }

    clear_frame_buffer(app_context); // Clear first
    
    rasterize_render_list(app_context, render_list);

    SDL_UnlockTexture(app_context->texture);
    SDL_RenderCopy(app_context->renderer, app_context->texture, NULL, NULL);
    SDL_RenderPresent(app_context->renderer);
    return 0;
}
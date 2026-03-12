#include <SDL2/SDL.h>
#include <assert.h>
#include <immintrin.h>

#include "../include/rasterizer.h"
#include "../include/types.h"
#include "../include/app.h"
#include "../include/coordinates.h"
#include "../include/logging.h"

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

void fill_triangle(RenderTriangle *triangle, AppContext *app_context, int tile_x, int tile_y, int tile_width, int tile_height) {
    // Triangle filling with barycentric coordinates


    Vector2f vertex0 = { (float)triangle->screen_positions[0].x, (float)triangle->screen_positions[0].y };
    Vector2f vertex1 = { (float)triangle->screen_positions[1].x, (float)triangle->screen_positions[1].y };
    Vector2f vertex2 = { (float)triangle->screen_positions[2].x, (float)triangle->screen_positions[2].y };

    float triangle_area = edge_function(vertex0, vertex1, vertex2);
    if (triangle_area >= 0) {
        return; // Skip degenerate triangles
    }
    __m128 inverse_triangle_area = _mm_set1_ps(1.0f / triangle_area);

    // Compute the bounding box of the triangle
    // min_x, min_y is the top left corner of the bounding box
    // max_x, max_y is the bottom right corner of the bounding box
    int min_x = CLAMP(MIN(MIN(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), tile_x, tile_x + tile_width - 1);
    int max_x = CLAMP(MAX(MAX(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), tile_x, tile_x + tile_width - 1);
    int min_y = CLAMP(MIN(MIN(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), tile_y, tile_y + tile_height - 1);
    int max_y = CLAMP(MAX(MAX(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), tile_y, tile_y + tile_height - 1);

    // Step values for w0, w1, and w2 to simplify the inner loop calculations.
    float weight0_step_x = vertex1.y - vertex2.y;
    float weight0_step_y = vertex2.x - vertex1.x;
    float weight1_step_x = vertex2.y - vertex0.y;
    float weight1_step_y = vertex0.x - vertex2.x;
    float weight2_step_x = vertex0.y - vertex1.y;
    float weight2_step_y = vertex1.x - vertex0.x;

    Vector2f start_point = { min_x + 0.5f, min_y + 0.5f };
    float weight0_block_row = edge_function(vertex1, vertex2, start_point);
    float weight1_block_row = edge_function(vertex2, vertex0, start_point);
    float weight2_block_row = edge_function(vertex0, vertex1, start_point);

    int block_size = 4;

    for (int block_y = min_y; block_y <= max_y; block_y += block_size) {
        float weight0_block = weight0_block_row;
        float weight1_block = weight1_block_row;
        float weight2_block = weight2_block_row;

        for (int block_x = min_x; block_x <= max_x; block_x += block_size) {
            float weight0_block_top_right = weight0_block + (block_size - 1) * weight0_step_x;
            float weight0_block_bottom_left = weight0_block + (block_size - 1) * weight0_step_y;
            float weight0_block_bottom_right = weight0_block + (block_size - 1) * weight0_step_x + (block_size - 1) * weight0_step_y;
            
            float weight1_block_top_right = weight1_block + (block_size - 1) * weight1_step_x;
            float weight1_block_bottom_left = weight1_block + (block_size -1) * weight1_step_y;
            float weight1_block_bottom_right = weight1_block + (block_size - 1) * weight1_step_x + (block_size - 1) * weight1_step_y;
            
            float weight2_block_top_right = weight2_block + (block_size - 1) * weight2_step_x;
            float weight2_block_bottom_left = weight2_block + (block_size - 1) * weight2_step_y;
            float weight2_block_bottom_right = weight2_block + (block_size - 1) * weight2_step_x + (block_size - 1) * weight2_step_y;

            bool weight0_all_inside = (weight0_block <= 0 && weight0_block_top_right <= 0 && weight0_block_bottom_left <= 0 && weight0_block_bottom_right <= 0);
            bool weight1_all_inside = (weight1_block <= 0 && weight1_block_top_right <= 0 && weight1_block_bottom_left <= 0 && weight1_block_bottom_right <= 0);
            bool weight2_all_inside = (weight2_block <= 0 && weight2_block_top_right <= 0 && weight2_block_bottom_left <= 0 && weight2_block_bottom_right <= 0);
            bool weight0_all_outside = (weight0_block > 0 && weight0_block_top_right > 0 && weight0_block_bottom_left > 0 && weight0_block_bottom_right > 0);
            bool weight1_all_outside = (weight1_block > 0 && weight1_block_top_right > 0 && weight1_block_bottom_left > 0 && weight1_block_bottom_right > 0);
            bool weight2_all_outside = (weight2_block > 0 && weight2_block_top_right > 0 && weight2_block_bottom_left > 0 && weight2_block_bottom_right > 0);

            if (weight0_all_outside || weight1_all_outside || weight2_all_outside) {
                // Skip this block since it's completely outside the triangle
            } else if (weight0_all_inside && weight1_all_inside && weight2_all_inside) {
                float weight0_pixel_row = weight0_block;
                float weight1_pixel_row = weight1_block;
                float weight2_pixel_row = weight2_block;
                for (int y = block_y; y < block_y + block_size && y <= max_y; y++) {
                    float weight0 = weight0_pixel_row;
                    float weight1 = weight1_pixel_row;
                    float weight2 = weight2_pixel_row;

                    // Pack 4 corner weights into SIMD registers
                    __m128 weight0_vec = _mm_set_ps(weight0 + 3 * weight0_step_x, weight0 + 2 * weight0_step_x, weight0 + weight0_step_x, weight0);
                    __m128 weight1_vec = _mm_set_ps(weight1 + 3 * weight1_step_x, weight1 + 2 * weight1_step_x, weight1 + weight1_step_x, weight1);
                    __m128 weight2_vec = _mm_set_ps(weight2 + 3 * weight2_step_x, weight2 + 2 * weight2_step_x, weight2 + weight2_step_x, weight2);

                    // Compute 4 barycentric coordinates in parallel
                    __m128 bary0_vec = _mm_mul_ps(weight0_vec, inverse_triangle_area);
                    __m128 bary1_vec = _mm_mul_ps(weight1_vec, inverse_triangle_area);
                    __m128 bary2_vec = _mm_mul_ps(weight2_vec, inverse_triangle_area);

                    // Compare 4 depth values in parallel
                    __m128 depth_vec = _mm_add_ps(_mm_add_ps(_mm_mul_ps(bary0_vec, _mm_set1_ps(triangle->depth_values[0])), _mm_mul_ps(bary1_vec, _mm_set1_ps(triangle->depth_values[1]))), _mm_mul_ps(bary2_vec, _mm_set1_ps(triangle->depth_values[2])));

                    // Retrieve the current depth buffer values for the 4 pixels
                    int index = y * app_context->window_resolution.x + block_x;
                    __m128 current_depth_vec = _mm_loadu_ps(&app_context->depth_buffer->depth_values[index]);
                    // See which of the new depths are closer to the camera
                    __m128 new_max_depth = _mm_cmplt_ps(depth_vec, current_depth_vec);

                    float depths[4];
                    _mm_storeu_ps(depths, depth_vec);

                    int mask = _mm_movemask_ps(new_max_depth);
                    for (int i = 0; i < 4; i++) {
                        if ((mask & (1 << i)) && (block_x + i <= max_x)) {
                            app_context->depth_buffer->depth_values[index + i] = depths[i];
                            app_context->frame_buffer[index + i] = triangle->color;
                            //draw_pixel_at_coordinates(app_context, block_x + i, y, triangle->color);
                        }
                    }

                    weight0_pixel_row += weight0_step_y;
                    weight1_pixel_row += weight1_step_y;
                    weight2_pixel_row += weight2_step_y;
                }
            } else {
                float weight0_pixel_row = weight0_block;
                float weight1_pixel_row = weight1_block;
                float weight2_pixel_row = weight2_block;
                for (int y = block_y; y < block_y + block_size && y <= max_y; y++) {
                    float weight0 = weight0_pixel_row;
                    float weight1 = weight1_pixel_row;
                    float weight2 = weight2_pixel_row;

                    __m128 weight0_vec = _mm_set_ps(weight0 + 3 * weight0_step_x, weight0 + 2 * weight0_step_x, weight0 + weight0_step_x, weight0);
                    __m128 weight1_vec = _mm_set_ps(weight1 + 3 * weight1_step_x, weight1 + 2 * weight1_step_x, weight1 + weight1_step_x, weight1);
                    __m128 weight2_vec = _mm_set_ps(weight2 + 3 * weight2_step_x, weight2 + 2 * weight2_step_x, weight2 + weight2_step_x, weight2);

                    __m128 zero = _mm_setzero_ps();
                    __m128 inside_mask = _mm_and_ps(_mm_and_ps(_mm_cmple_ps(weight0_vec, zero), _mm_cmple_ps(weight1_vec, zero)), _mm_cmple_ps(weight2_vec, zero));

                    __m128 bary0_vec = _mm_mul_ps(weight0_vec, inverse_triangle_area);
                    __m128 bary1_vec = _mm_mul_ps(weight1_vec, inverse_triangle_area);
                    __m128 bary2_vec = _mm_mul_ps(weight2_vec, inverse_triangle_area);

                    __m128 depth_vec = _mm_add_ps(_mm_add_ps(_mm_mul_ps(bary0_vec, _mm_set1_ps(triangle->depth_values[0])), _mm_mul_ps(bary1_vec, _mm_set1_ps(triangle->depth_values[1]))), _mm_mul_ps(bary2_vec, _mm_set1_ps(triangle->depth_values[2])));

                    float depths[4];
                    _mm_storeu_ps(depths, depth_vec);

                    int index = y * app_context->window_resolution.x + block_x;
                    __m128 current_depth_vec = _mm_loadu_ps(&app_context->depth_buffer->depth_values[index]);

                    __m128 depth_mask = _mm_cmplt_ps(depth_vec, current_depth_vec);

                    __m128 final_mask = _mm_and_ps(inside_mask, depth_mask);

                    int mask = _mm_movemask_ps(final_mask);

                    for (int i = 0; i < 4; i++) {
                        if ((mask & (1 << i)) && (block_x + i <= max_x)) {
                            app_context->depth_buffer->depth_values[index + i] = depths[i];
                            draw_pixel_at_coordinates(app_context, block_x + i, y, triangle->color);
                        }
                    }
                    weight0_pixel_row += weight0_step_y;
                    weight1_pixel_row += weight1_step_y;
                    weight2_pixel_row += weight2_step_y;
                }
            }
            weight0_block += block_size * weight0_step_x;
            weight1_block += block_size * weight1_step_x;
            weight2_block += block_size * weight2_step_x;
        }
        weight0_block_row += block_size * weight0_step_y;
        weight1_block_row += block_size * weight1_step_y;
        weight2_block_row += block_size * weight2_step_y;
    }
}

void rasterizer_worker(void *arg) {
    RasterizerJob *job = (RasterizerJob*)arg;
    for (int t = 0; t < job->triangle_count; t++) {
        RenderTriangle *triangle = job->render_triangles[t];
        fill_triangle(triangle, job->app_context, job->tile_x, job->tile_y, job->tile_width, job->tile_height);
    }
    return;
}

int render(AppContext *app_context, RenderList *render_list) {
    int texture_lock_result = SDL_LockTexture(app_context->texture, NULL, (void**)&app_context->frame_buffer, &(int){0});
    if (texture_lock_result != 0) {
        LOG_ERROR("Error locking SDL texture: %s", SDL_GetError());
        return 1;
    }

    clear_frame_buffer(app_context); // Clear first


    // Go through the render list and figure out how to divide the triangles into tiles and create rasterizer jobs for each tile
    int total_tiles = app_context->tiles_x * app_context->tiles_y;
    //RenderTriangle ***tile_tris = malloc(total_tiles * sizeof(RenderTriangle**));
    RenderTriangle ***tile_triangles = app_context->tile_triangles;
    int *tile_counts = app_context->tile_counts;
    int *tile_caps = app_context->tile_caps;
    memset(tile_counts, 0, total_tiles * sizeof(int));

    for (int t = 0; t < render_list->triangle_count; t++) {
        RenderTriangle *triangle = &render_list->triangles[t];
        // Compute the bounding box of the triangle
        int min_x = MAX(MIN(MIN(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), 0);
        int max_x = MIN(MAX(MAX(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), app_context->window_resolution.x - 1);
        int min_y = MAX(MIN(MIN(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), 0);
        int max_y = MIN(MAX(MAX(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), app_context->window_resolution.y - 1);

        int tile_start_x = min_x / app_context->tile_size;
        int tile_end_x = max_x / app_context->tile_size;
        int tile_start_y = min_y / app_context->tile_size;
        int tile_end_y = max_y / app_context->tile_size;

        for (int ty = tile_start_y; ty <= tile_end_y; ty++) {
            for (int tx = tile_start_x; tx <= tile_end_x; tx++) {
                int tile_index = ty * app_context->tiles_x + tx;
                if (tile_counts[tile_index] >= tile_caps[tile_index]) {
                    tile_caps[tile_index] *= 2;
                    tile_triangles[tile_index] = realloc(tile_triangles[tile_index], tile_caps[tile_index] * sizeof(RenderTriangle*));
                    if (!tile_triangles[tile_index]) {
                        LOG_ERROR("Failed to reallocate memory for tile triangles");
                        exit(EXIT_FAILURE);
                    }
                }
                tile_triangles[tile_index][tile_counts[tile_index]++] = triangle;
            }
        }
    }

    // Submit jobs for non-empty tiles
    int job_count = 0;
    //RasterizerJob *jobs[total_tiles];
    for (int ty = 0; ty < app_context->tiles_y; ty++) {
        for (int tx = 0; tx < app_context->tiles_x; tx++) {
            int tile_index = ty * app_context->tiles_x + tx;
            if (tile_counts[tile_index] > 0) {
                RasterizerJob *job = &app_context->rasterizer_job_pool[job_count];
                job->app_context = app_context;
                job->render_triangles = tile_triangles[tile_index];
                job->tile_x = tx * app_context->tile_size;
                job->tile_y = ty * app_context->tile_size;
                job->tile_width = MIN(app_context->tile_size, app_context->window_resolution.x - tx * app_context->tile_size);
                job->tile_height = MIN(app_context->tile_size, app_context->window_resolution.y - ty * app_context->tile_size);
                job->triangle_count = tile_counts[tile_index];
                submit_job_to_thread_pool(&app_context->thread_pool, rasterizer_worker, job);
                job_count++;
            }
        }
    }
    
    // Wait for all rasterizer jobs to finish
    thread_pool_wait_for_completion(&app_context->thread_pool);


    SDL_UnlockTexture(app_context->texture);
    SDL_RenderCopy(app_context->renderer, app_context->texture, NULL, NULL);
    SDL_RenderPresent(app_context->renderer);
    return 0;
}

void initialize_rasterizer_job_pool(AppContext *app_context, int tile_size) {
    LOG_INFO("Creating rasterizer job pool with tile size %d", tile_size);
    app_context->tile_size = tile_size;
    app_context->tiles_x = (app_context->window_resolution.x + tile_size - 1) / tile_size;
    app_context->tiles_y = (app_context->window_resolution.y + tile_size - 1) / tile_size;
    int total_tiles = app_context->tiles_x * app_context->tiles_y;
    app_context->tile_triangles = malloc(total_tiles * sizeof(RenderTriangle**));
    app_context->tile_counts = calloc(total_tiles, sizeof(int));
    app_context->tile_caps = malloc(total_tiles * sizeof(int));
    app_context->total_tiles = total_tiles;
    for (int i = 0; i < total_tiles; i++) {
        app_context->tile_caps[i] = 16;
        app_context->tile_triangles[i] = malloc(16 * sizeof(RenderTriangle*));
    }
    if (!app_context->tile_triangles || !app_context->tile_counts || !app_context->tile_caps) {
        LOG_ERROR("Failed to allocate memory for tile triangle arrays");
        exit(EXIT_FAILURE);
    }
    app_context->rasterizer_job_pool = malloc(total_tiles * sizeof(RasterizerJob));
    if (!app_context->rasterizer_job_pool) {
        LOG_ERROR("Failed to allocate memory for rasterizer job pool");
        exit(EXIT_FAILURE);
    }
}
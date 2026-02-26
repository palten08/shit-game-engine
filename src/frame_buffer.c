#include "../include/frame_buffer.h"
#include "../include/types.h"

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

/**
 * @brief Iterates over the scene and draws all the squares and lines contained within it.
 * @param app_context A pointer to the application context.
 * @param scene A pointer to the scene to be rendered.
 * @return 0 on success.
 */
int write_scene_to_frame_buffer(AppContext *app_context, Scene *scene) {
    for (int i = 0; i < scene->square_count; i++) {
        SquareData2D square = scene->squares[i];
        // Draw the vertices and the edges at the same time by just looking ahead to the next vert and using that as the end point
        for (int v = 0; v < 4; v++) {
            VertexData vertex = square.vertices[v];
            VertexData next_vertex = square.vertices[(v + 1) % 4]; // Get the next vertex in the square (wrap around to the first vertex after the last one)
            draw_pixel_at_coordinates(app_context, vertex.position.x, vertex.position.y, vertex.color);
            draw_line_between_coordinates(app_context, vertex.position.x, vertex.position.y, next_vertex.position.x, next_vertex.position.y, vertex.color);
        }
    }
    for (int i = 0; i < scene->line_count; i++) {
        LineData2D line = scene->lines[i];
        // Draw any lines in the scene
        draw_line_between_coordinates(app_context, line.start.x, line.start.y, line.end.x, line.end.y, line.color);
        //draw_line_between_coordinates(app_context, line.start.x, line.start.y, line.end.x, line.end.y, line.color);
    }
    return 0;
}
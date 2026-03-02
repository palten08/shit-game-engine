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
    for (int i = 0; i < scene->cube_count; i++) {
        // Stupid simple orthographic projection for right now
        CubeData3D cube = scene->cubes[i];
        // Draw the vertices and the edges at the same time by just looking ahead to the next vert and using that as the end point
        for (int v = 0; v < 8; v++) {
            VertexData3D vertex = cube.vertices[v];
            
            // Connect the vertices to form the edges of the cube
            // Front face
            if (v < 4) {
                VertexData3D next_vertex = cube.vertices[(v + 1) % 4]; // Get the next vertex in the front face (wrap around to the first vertex after the last one)
                if (!vertex.is_visible || !next_vertex.is_visible) {
                    continue; // Don't draw edges for any vertex that is not visible
                }
                float projected_x = vertex.position.x;
                float projected_y = vertex.position.y;
                float projected_next_x = next_vertex.position.x;
                float projected_next_y = next_vertex.position.y;

                draw_pixel_at_coordinates(app_context, projected_x, projected_y, vertex.color);
                //draw_line_between_coordinates(app_context, projected_x, projected_y, projected_next_x, projected_next_y, vertex.color);
                
                // Connect front and back faces
                VertexData3D corresponding_vertex = cube.vertices[(v % 4) + ((v < 4) ? 4 : -4)]; // Get the corresponding vertex in the other face
                if (corresponding_vertex.is_visible) {
                    float projected_corresponding_x = corresponding_vertex.position.x;
                    float projected_corresponding_y = corresponding_vertex.position.y;
                    //draw_line_between_coordinates(app_context, projected_x, projected_y, projected_corresponding_x, projected_corresponding_y, vertex.color);
                }
            }
            // Back face
            else {
                VertexData3D next_vertex = cube.vertices[4 + ((v - 4 + 1) % 4)]; // Get the next vertex in the back face (wrap around to the first vertex after the last one)
                if (!vertex.is_visible ||!next_vertex.is_visible) {
                    continue; // Don't draw edges for any vertex that is not visible
                }
                float projected_x = vertex.position.x;
                float projected_y = vertex.position.y;
                float projected_next_x = next_vertex.position.x;
                float projected_next_y = next_vertex.position.y;
                //printf("Back face projected X and Y coordinates for vertex %d: (%f, %f)\n", v, projected_x, projected_y);
                draw_pixel_at_coordinates(app_context, projected_x, projected_y, vertex.color);
                //draw_line_between_coordinates(app_context, projected_x, projected_y, projected_next_x, projected_next_y, vertex.color);
            }
        }

        // Fill faces
        for (int f = 0; f < 6; f++) {

            QuadData3D face = cube.faces[f];
            for (int t = 0; t < 2; t++) {
                TriangleData3D triangle = face.triangles[t];
                VertexData3D vertex1 = cube.vertices[triangle.vertex_indices[0]];
                VertexData3D vertex2 = cube.vertices[triangle.vertex_indices[1]];
                VertexData3D vertex3 = cube.vertices[triangle.vertex_indices[2]];
                if (!vertex1.is_visible || !vertex2.is_visible || !vertex3.is_visible) {
                    continue; // Don't draw triangles for any vertex that is not visible
                }

                uint32_t triangle_color = triangle.color;

                // Draw the edges of the triangle
                float projected_x1 = vertex1.position.x;
                float projected_y1 = vertex1.position.y;
                float projected_x2 = vertex2.position.x;
                float projected_y2 = vertex2.position.y;
                float projected_x3 = vertex3.position.x;
                float projected_y3 = vertex3.position.y;
                draw_line_between_coordinates(app_context, projected_x1, projected_y1, projected_x2, projected_y2, triangle_color);
                draw_line_between_coordinates(app_context, projected_x2, projected_y2, projected_x3, projected_y3, triangle_color);
                draw_line_between_coordinates(app_context, projected_x3, projected_y3, projected_x1, projected_y1, triangle_color);

                // Fill in the triangle using a simple scanline fill algorithm
                // Find the bounding box of the triangle
                int min_x = (int)fminf(fminf(projected_x1, projected_x2), projected_x3);
                int max_x = (int)fmaxf(fmaxf(projected_x1, projected_x2), projected_x3);
                int min_y = (int)fminf(fminf(projected_y1, projected_y2), projected_y3);
                int max_y = (int)fmaxf(fmaxf(projected_y1, projected_y2), projected_y3);
                // Iterate over the bounding box and fill in the pixels that are inside the triangle
                for (int y = min_y; y <= max_y; y++) {
                    for (int x = min_x; x <= max_x; x++) {
                        // Barycentric coordinates to determine if the pixel is inside the triangle
                        float alpha = ((projected_y2 - projected_y3) * (x - projected_x3) + (projected_x3 - projected_x2) * (y - projected_y3)) / ((projected_y2 - projected_y3) * (projected_x1 - projected_x3) + (projected_x3 - projected_x2) * (projected_y1 - projected_y3));
                        float beta = ((projected_y3 - projected_y1) * (x - projected_x3) + (projected_x1 - projected_x3) * (y - projected_y3)) / ((projected_y2 - projected_y3) * (projected_x1 - projected_x3) + (projected_x3 - projected_x2) * (projected_y1 - projected_y3));
                        float gamma = 1.0f - alpha - beta;
                        if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                            draw_pixel_at_coordinates(app_context, x, y, triangle_color);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
#include <SDL3/SDL.h>
#include <assert.h>
#include <immintrin.h>
#include <math.h>

#include "../include/rasterizer.h"
#include "../include/types.h"
#include "../include/app.h"
#include "../include/coordinates.h"
#include "../include/logging.h"
#include "../include/utils.h"
#include "../include/matrix_operations.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MAX((lo), MIN((x), (hi))))

/**
 * @brief Sets the entire frame buffer to a specific color.
 * @param app_context A pointer to the application context.
 * @param color The color to set the frame buffer to.
 * @return 0 on success.
 */
int set_frame_buffer(AppContext *app_context, RGBVector3f color) {
    uint32_t hex_color = convert_rgbvec3f_to_hex_color(color);
    for (int y = 0; y < app_context->window_resolution.y; y++) {
        for (int x = 0; x < app_context->window_resolution.x; x++) {
            app_context->frame_buffer[y * app_context->window_resolution.x + x] = hex_color;
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
    return set_frame_buffer(app_context, (RGBVector3f){0.0f, 0.0f, 0.0f}); // Clear to black
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
int draw_pixel_at_coordinates(AppContext *app_context, int coordinate_x, int coordinate_y, RGBVector3f color) {
    uint32_t hex_color = convert_rgbvec3f_to_hex_color(color);
    int frame_buffer_position_x = coordinate_x;
    int frame_buffer_position_y = coordinate_y;
    if (frame_buffer_position_x < 0 || frame_buffer_position_x >= app_context->window_resolution.x || frame_buffer_position_y < 0 || frame_buffer_position_y >= app_context->window_resolution.y) {
        return 1; // Skip pixels that are outside the window boundaries
    }
    app_context->frame_buffer[frame_buffer_position_y * app_context->window_resolution.x + frame_buffer_position_x] = hex_color;
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
int draw_horizontal_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color) {
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
int draw_vertical_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color) {
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
int draw_line_between_coordinates(AppContext *app_context, int x1, int y1, int x2, int y2, RGBVector3f color) {
    if (abs(x2 - x1) > abs(y2 - y1)) {
        return draw_horizontal_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    } else {
        return draw_vertical_line_between_coordinates(app_context, x1, y1, x2, y2, color);
    }
}

float edge_function(Vector2f a, Vector2f b, Vector2f c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/**
 * @brief Computes per-pixel Blinn-Phong lighting for a single pixel.
 * All input and output color values are in 0.0-1.0 range.
 * @param out_red Pointer to the output red channel value.
 * @param out_green Pointer to the output green channel value.
 * @param out_blue Pointer to the output blue channel value.
 * @param diffuse_red The diffuse (material) red channel value for this pixel.
 * @param diffuse_green The diffuse (material) green channel value for this pixel.
 * @param diffuse_blue The diffuse (material) blue channel value for this pixel.
 * @param normal_x The x component of the surface normal at this pixel.
 * @param normal_y The y component of the surface normal at this pixel.
 * @param normal_z The z component of the surface normal at this pixel.
 * @param world_position_x The x component of the world-space position of this pixel.
 * @param world_position_y The y component of the world-space position of this pixel.
 * @param world_position_z The z component of the world-space position of this pixel.
 * @param light_direction_x The x component of the normalized light direction (toward the light).
 * @param light_direction_y The y component of the normalized light direction (toward the light).
 * @param light_direction_z The z component of the normalized light direction (toward the light).
 * @param scene The scene containing the directional light and camera information.
 * @param material The material containing Phong shading parameters (specular color, exponent).
 */
static void shade_blinn_phong(
    float *out_red, float *out_green, float *out_blue,
    float diffuse_red, float diffuse_green, float diffuse_blue,
    float normal_x, float normal_y, float normal_z,
    float world_position_x, float world_position_y, float world_position_z,
    float light_direction_x, float light_direction_y, float light_direction_z,
    Scene *scene, Material *material)
{
    float normal_dot_light = normal_x * light_direction_x + normal_y * light_direction_y + normal_z * light_direction_z;
    float diffuse_intensity = normal_dot_light > 0.0f ? normal_dot_light : 0.0f;

    float specular_intensity = 0.0f;
    if (diffuse_intensity > 0.001f) {
        float view_direction_x = scene->virtual_camera.position.x - world_position_x;
        float view_direction_y = scene->virtual_camera.position.y - world_position_y;
        float view_direction_z = scene->virtual_camera.position.z - world_position_z;
        float view_direction_length = sqrtf(view_direction_x * view_direction_x + view_direction_y * view_direction_y + view_direction_z * view_direction_z);
        if (view_direction_length > 0.0001f) {
            view_direction_x /= view_direction_length;
            view_direction_y /= view_direction_length;
            view_direction_z /= view_direction_length;
            float half_vector_x = light_direction_x + view_direction_x;
            float half_vector_y = light_direction_y + view_direction_y;
            float half_vector_z = light_direction_z + view_direction_z;
            float half_vector_length = sqrtf(half_vector_x * half_vector_x + half_vector_y * half_vector_y + half_vector_z * half_vector_z);
            if (half_vector_length > 0.0001f) {
                half_vector_x /= half_vector_length;
                half_vector_y /= half_vector_length;
                half_vector_z /= half_vector_length;
                float normal_dot_half = normal_x * half_vector_x + normal_y * half_vector_y + normal_z * half_vector_z;
                if (normal_dot_half > 0.0f)
                    specular_intensity = powf(normal_dot_half, material->phong_parameters->specular_exponent);
            }
        }
    }

    float ambient = scene->directional_light.ambient_intensity;
    float lighting = ambient + (1.0f - ambient) * diffuse_intensity * scene->directional_light.intensity;

    *out_red = CLAMP(lighting * diffuse_red + diffuse_intensity * material->phong_parameters->specular_color.r * specular_intensity, 0.0f, 1.0f);
    *out_green = CLAMP(lighting * diffuse_green + diffuse_intensity * material->phong_parameters->specular_color.g * specular_intensity, 0.0f, 1.0f);
    *out_blue = CLAMP(lighting * diffuse_blue + diffuse_intensity * material->phong_parameters->specular_color.b * specular_intensity, 0.0f, 1.0f);
}

// Macro for SIMD barycentric interpolation of a 3-component attribute into separate x/y/z arrays for 4 pixels
#define INTERPOLATE_ATTRIBUTE_3D(out_x, out_y, out_z, barycentric_0, barycentric_1, barycentric_2, \
    vertex0_x, vertex0_y, vertex0_z, vertex1_x, vertex1_y, vertex1_z, vertex2_x, vertex2_y, vertex2_z) \
    _mm_storeu_ps(out_x, _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex0_x), _mm_mul_ps(barycentric_1, vertex1_x)), _mm_mul_ps(barycentric_2, vertex2_x))); \
    _mm_storeu_ps(out_y, _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex0_y), _mm_mul_ps(barycentric_1, vertex1_y)), _mm_mul_ps(barycentric_2, vertex2_y))); \
    _mm_storeu_ps(out_z, _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex0_z), _mm_mul_ps(barycentric_1, vertex1_z)), _mm_mul_ps(barycentric_2, vertex2_z)));

// Macro for SIMD barycentric interpolation of a UV attribute into separate u/v arrays for 4 pixels
#define INTERPOLATE_ATTRIBUTE_2D(out_u, out_v, barycentric_0, barycentric_1, barycentric_2, \
    vertex0_u, vertex0_v, vertex1_u, vertex1_v, vertex2_u, vertex2_v) \
    _mm_storeu_ps(out_u, _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex0_u), _mm_mul_ps(barycentric_1, vertex1_u)), _mm_mul_ps(barycentric_2, vertex2_u))); \
    _mm_storeu_ps(out_v, _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex0_v), _mm_mul_ps(barycentric_1, vertex1_v)), _mm_mul_ps(barycentric_2, vertex2_v)));

void fill_triangle(RenderTriangle *triangle, AppContext *app_context, int tile_x, int tile_y, int tile_width, int tile_height, Scene *scene) {
    Vector2f screen_vertex_0 = { (float)triangle->screen_positions[0].x, (float)triangle->screen_positions[0].y };
    Vector2f screen_vertex_1 = { (float)triangle->screen_positions[1].x, (float)triangle->screen_positions[1].y };
    Vector2f screen_vertex_2 = { (float)triangle->screen_positions[2].x, (float)triangle->screen_positions[2].y };

    // Broadcast per-vertex color components for SIMD interpolation
    __m128 vertex_0_color_red = _mm_set1_ps(triangle->vertex_colors[0].r);
    __m128 vertex_0_color_green = _mm_set1_ps(triangle->vertex_colors[0].g);
    __m128 vertex_0_color_blue = _mm_set1_ps(triangle->vertex_colors[0].b);
    __m128 vertex_1_color_red = _mm_set1_ps(triangle->vertex_colors[1].r);
    __m128 vertex_1_color_green = _mm_set1_ps(triangle->vertex_colors[1].g);
    __m128 vertex_1_color_blue = _mm_set1_ps(triangle->vertex_colors[1].b);
    __m128 vertex_2_color_red = _mm_set1_ps(triangle->vertex_colors[2].r);
    __m128 vertex_2_color_green = _mm_set1_ps(triangle->vertex_colors[2].g);
    __m128 vertex_2_color_blue = _mm_set1_ps(triangle->vertex_colors[2].b);

    // Broadcast per-vertex world-space position components for SIMD interpolation
    __m128 vertex_0_world_position_x = _mm_set1_ps(triangle->world_space_vertex_positions[0].x);
    __m128 vertex_0_world_position_y = _mm_set1_ps(triangle->world_space_vertex_positions[0].y);
    __m128 vertex_0_world_position_z = _mm_set1_ps(triangle->world_space_vertex_positions[0].z);
    __m128 vertex_1_world_position_x = _mm_set1_ps(triangle->world_space_vertex_positions[1].x);
    __m128 vertex_1_world_position_y = _mm_set1_ps(triangle->world_space_vertex_positions[1].y);
    __m128 vertex_1_world_position_z = _mm_set1_ps(triangle->world_space_vertex_positions[1].z);
    __m128 vertex_2_world_position_x = _mm_set1_ps(triangle->world_space_vertex_positions[2].x);
    __m128 vertex_2_world_position_y = _mm_set1_ps(triangle->world_space_vertex_positions[2].y);
    __m128 vertex_2_world_position_z = _mm_set1_ps(triangle->world_space_vertex_positions[2].z);

    // Broadcast per-vertex world-space normal components for SIMD interpolation
    __m128 vertex_0_world_normal_x = _mm_set1_ps(triangle->world_space_vertex_normals[0].x);
    __m128 vertex_0_world_normal_y = _mm_set1_ps(triangle->world_space_vertex_normals[0].y);
    __m128 vertex_0_world_normal_z = _mm_set1_ps(triangle->world_space_vertex_normals[0].z);
    __m128 vertex_1_world_normal_x = _mm_set1_ps(triangle->world_space_vertex_normals[1].x);
    __m128 vertex_1_world_normal_y = _mm_set1_ps(triangle->world_space_vertex_normals[1].y);
    __m128 vertex_1_world_normal_z = _mm_set1_ps(triangle->world_space_vertex_normals[1].z);
    __m128 vertex_2_world_normal_x = _mm_set1_ps(triangle->world_space_vertex_normals[2].x);
    __m128 vertex_2_world_normal_y = _mm_set1_ps(triangle->world_space_vertex_normals[2].y);
    __m128 vertex_2_world_normal_z = _mm_set1_ps(triangle->world_space_vertex_normals[2].z);

    // Broadcast per-vertex UV components for SIMD interpolation
    __m128 vertex_0_uv_u = _mm_set1_ps(triangle->uv_coordinates[0].x);
    __m128 vertex_0_uv_v = _mm_set1_ps(triangle->uv_coordinates[0].y);
    __m128 vertex_1_uv_u = _mm_set1_ps(triangle->uv_coordinates[1].x);
    __m128 vertex_1_uv_v = _mm_set1_ps(triangle->uv_coordinates[1].y);
    __m128 vertex_2_uv_u = _mm_set1_ps(triangle->uv_coordinates[2].x);
    __m128 vertex_2_uv_v = _mm_set1_ps(triangle->uv_coordinates[2].y);

    __m128 vertex_0_perspective_w = _mm_set1_ps(triangle->perspective_w_values[0]);
    __m128 vertex_1_perspective_w = _mm_set1_ps(triangle->perspective_w_values[1]);
    __m128 vertex_2_perspective_w = _mm_set1_ps(triangle->perspective_w_values[2]);

    // Something about 1/w
    __m128 vertex_0_one_over_perspective_w = _mm_div_ps(_mm_set1_ps(1.0f), vertex_0_perspective_w);
    __m128 vertex_1_one_over_perspective_w = _mm_div_ps(_mm_set1_ps(1.0f), vertex_1_perspective_w);
    __m128 vertex_2_one_over_perspective_w = _mm_div_ps(_mm_set1_ps(1.0f), vertex_2_perspective_w);

    // Divide each vertex's UV coordinates by its clip-space w for perspective-correct interpolation
    vertex_0_uv_u = _mm_div_ps(vertex_0_uv_u, vertex_0_perspective_w);
    vertex_0_uv_v = _mm_div_ps(vertex_0_uv_v, vertex_0_perspective_w);
    vertex_1_uv_u = _mm_div_ps(vertex_1_uv_u, vertex_1_perspective_w);
    vertex_1_uv_v = _mm_div_ps(vertex_1_uv_v, vertex_1_perspective_w);
    vertex_2_uv_u = _mm_div_ps(vertex_2_uv_u, vertex_2_perspective_w);
    vertex_2_uv_v = _mm_div_ps(vertex_2_uv_v, vertex_2_perspective_w);


    // Compute and normalize the light direction (pointing toward the light source)
    float light_direction_x = -scene->directional_light.direction.x;
    float light_direction_y = -scene->directional_light.direction.y;
    float light_direction_z = -scene->directional_light.direction.z;
    float light_direction_length = sqrtf(light_direction_x * light_direction_x + light_direction_y * light_direction_y + light_direction_z * light_direction_z);
    if (light_direction_length > 0.0f) {
        light_direction_x /= light_direction_length;
        light_direction_y /= light_direction_length;
        light_direction_z /= light_direction_length;
    }

    int use_blinn_phong_shading = triangle->material && triangle->material->shading_model == SHADING_PHONG && triangle->material->phong_parameters;

    float triangle_area = edge_function(screen_vertex_0, screen_vertex_1, screen_vertex_2);
    if (triangle_area >= 0) return;
    __m128 inverse_triangle_area = _mm_set1_ps(1.0f / triangle_area);

    int bounding_box_min_x = CLAMP(MIN(MIN(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), tile_x, tile_x + tile_width - 1);
    int bounding_box_max_x = CLAMP(MAX(MAX(triangle->screen_positions[0].x, triangle->screen_positions[1].x), triangle->screen_positions[2].x), tile_x, tile_x + tile_width - 1);
    int bounding_box_min_y = CLAMP(MIN(MIN(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), tile_y, tile_y + tile_height - 1);
    int bounding_box_max_y = CLAMP(MAX(MAX(triangle->screen_positions[0].y, triangle->screen_positions[1].y), triangle->screen_positions[2].y), tile_y, tile_y + tile_height - 1);

    float weight_0_step_x = screen_vertex_1.y - screen_vertex_2.y;
    float weight_0_step_y = screen_vertex_2.x - screen_vertex_1.x;
    float weight_1_step_x = screen_vertex_2.y - screen_vertex_0.y;
    float weight_1_step_y = screen_vertex_0.x - screen_vertex_2.x;
    float weight_2_step_x = screen_vertex_0.y - screen_vertex_1.y;
    float weight_2_step_y = screen_vertex_1.x - screen_vertex_0.x;

    Vector2f start_point = { bounding_box_min_x + 0.5f, bounding_box_min_y + 0.5f };
    float weight_0_block_row = edge_function(screen_vertex_1, screen_vertex_2, start_point);
    float weight_1_block_row = edge_function(screen_vertex_2, screen_vertex_0, start_point);
    float weight_2_block_row = edge_function(screen_vertex_0, screen_vertex_1, start_point);

    int block_size = 4;

    for (int block_y = bounding_box_min_y; block_y <= bounding_box_max_y; block_y += block_size) {
        float weight_0_block = weight_0_block_row;
        float weight_1_block = weight_1_block_row;
        float weight_2_block = weight_2_block_row;

        for (int block_x = bounding_box_min_x; block_x <= bounding_box_max_x; block_x += block_size) {
            // Evaluate edge weights at block corners for early accept/reject
            float weight_0_top_right = weight_0_block + (block_size - 1) * weight_0_step_x;
            float weight_0_bottom_left = weight_0_block + (block_size - 1) * weight_0_step_y;
            float weight_0_bottom_right = weight_0_block + (block_size - 1) * weight_0_step_x + (block_size - 1) * weight_0_step_y;
            float weight_1_top_right = weight_1_block + (block_size - 1) * weight_1_step_x;
            float weight_1_bottom_left = weight_1_block + (block_size - 1) * weight_1_step_y;
            float weight_1_bottom_right = weight_1_block + (block_size - 1) * weight_1_step_x + (block_size - 1) * weight_1_step_y;
            float weight_2_top_right = weight_2_block + (block_size - 1) * weight_2_step_x;
            float weight_2_bottom_left = weight_2_block + (block_size - 1) * weight_2_step_y;
            float weight_2_bottom_right = weight_2_block + (block_size - 1) * weight_2_step_x + (block_size - 1) * weight_2_step_y;

            bool weight_0_all_inside = (weight_0_block <= 0 && weight_0_top_right <= 0 && weight_0_bottom_left <= 0 && weight_0_bottom_right <= 0);
            bool weight_1_all_inside = (weight_1_block <= 0 && weight_1_top_right <= 0 && weight_1_bottom_left <= 0 && weight_1_bottom_right <= 0);
            bool weight_2_all_inside = (weight_2_block <= 0 && weight_2_top_right <= 0 && weight_2_bottom_left <= 0 && weight_2_bottom_right <= 0);
            bool weight_0_all_outside = (weight_0_block > 0 && weight_0_top_right > 0 && weight_0_bottom_left > 0 && weight_0_bottom_right > 0);
            bool weight_1_all_outside = (weight_1_block > 0 && weight_1_top_right > 0 && weight_1_bottom_left > 0 && weight_1_bottom_right > 0);
            bool weight_2_all_outside = (weight_2_block > 0 && weight_2_top_right > 0 && weight_2_bottom_left > 0 && weight_2_bottom_right > 0);

            if (weight_0_all_outside || weight_1_all_outside || weight_2_all_outside) {
                // Block is entirely outside the triangle — skip it
            } else if (weight_0_all_inside && weight_1_all_inside && weight_2_all_inside) {
                // All inside
                float weight_0_pixel_row = weight_0_block;
                float weight_1_pixel_row = weight_1_block;
                float weight_2_pixel_row = weight_2_block;
                for (int y = block_y; y < block_y + block_size && y <= bounding_box_max_y; y++) {
                    __m128 barycentric_0 = _mm_mul_ps(_mm_set_ps(weight_0_pixel_row + 3 * weight_0_step_x, weight_0_pixel_row + 2 * weight_0_step_x, weight_0_pixel_row + weight_0_step_x, weight_0_pixel_row), inverse_triangle_area);
                    __m128 barycentric_1 = _mm_mul_ps(_mm_set_ps(weight_1_pixel_row + 3 * weight_1_step_x, weight_1_pixel_row + 2 * weight_1_step_x, weight_1_pixel_row + weight_1_step_x, weight_1_pixel_row), inverse_triangle_area);
                    __m128 barycentric_2 = _mm_mul_ps(_mm_set_ps(weight_2_pixel_row + 3 * weight_2_step_x, weight_2_pixel_row + 2 * weight_2_step_x, weight_2_pixel_row + weight_2_step_x, weight_2_pixel_row), inverse_triangle_area);

                    __m128 interpolated_depth = _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, _mm_set1_ps(triangle->depth_values[0])), _mm_mul_ps(barycentric_1, _mm_set1_ps(triangle->depth_values[1]))), _mm_mul_ps(barycentric_2, _mm_set1_ps(triangle->depth_values[2])));
                    int frame_buffer_index = y * app_context->window_resolution.x + block_x;
                    int depth_test_mask = _mm_movemask_ps(_mm_cmplt_ps(interpolated_depth, _mm_loadu_ps(&app_context->depth_buffer->depth_values[frame_buffer_index])));
                    float pixel_depths[4];
                    _mm_storeu_ps(pixel_depths, interpolated_depth);

                    float pixel_red[4], pixel_green[4], pixel_blue[4];
                    INTERPOLATE_ATTRIBUTE_3D(pixel_red, pixel_green, pixel_blue, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_color_red, vertex_0_color_green, vertex_0_color_blue,
                        vertex_1_color_red, vertex_1_color_green, vertex_1_color_blue,
                        vertex_2_color_red, vertex_2_color_green, vertex_2_color_blue);

                    float pixel_world_position_x[4], pixel_world_position_y[4], pixel_world_position_z[4];
                    float pixel_world_normal_x[4], pixel_world_normal_y[4], pixel_world_normal_z[4];
                    INTERPOLATE_ATTRIBUTE_3D(pixel_world_position_x, pixel_world_position_y, pixel_world_position_z, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_world_position_x, vertex_0_world_position_y, vertex_0_world_position_z,
                        vertex_1_world_position_x, vertex_1_world_position_y, vertex_1_world_position_z,
                        vertex_2_world_position_x, vertex_2_world_position_y, vertex_2_world_position_z);
                    INTERPOLATE_ATTRIBUTE_3D(pixel_world_normal_x, pixel_world_normal_y, pixel_world_normal_z, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_world_normal_x, vertex_0_world_normal_y, vertex_0_world_normal_z,
                        vertex_1_world_normal_x, vertex_1_world_normal_y, vertex_1_world_normal_z,
                        vertex_2_world_normal_x, vertex_2_world_normal_y, vertex_2_world_normal_z);

                    float pixel_uv_u[4], pixel_uv_v[4];
                    INTERPOLATE_ATTRIBUTE_2D(pixel_uv_u, pixel_uv_v, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_uv_u, vertex_0_uv_v,
                        vertex_1_uv_u, vertex_1_uv_v,
                        vertex_2_uv_u, vertex_2_uv_v);

                    float perspective_correction_factor[4];
                    _mm_storeu_ps(perspective_correction_factor, _mm_div_ps(_mm_set1_ps(1.0f), _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex_0_one_over_perspective_w), _mm_mul_ps(barycentric_1, vertex_1_one_over_perspective_w)), _mm_mul_ps(barycentric_2, vertex_2_one_over_perspective_w))));


                    for (int i = 0; i < 4; i++) {
                        if ((depth_test_mask & (1 << i)) && (block_x + i <= bounding_box_max_x)) {
                            app_context->depth_buffer->depth_values[frame_buffer_index + i] = pixel_depths[i];
                            // If the material has a texture we sample it to get the effect it has on the diffuse color for this pixel
                            if (triangle->material && triangle->material->diffuse_texture) {
                                Texture *diffuse_texture = triangle->material->diffuse_texture;

                                // Recover the correct UV by dividing by the interpolated UV's perspective correction factor (which is 1/w interpolated across the triangle)
                                pixel_uv_u[i] *= perspective_correction_factor[i];
                                pixel_uv_v[i] *= perspective_correction_factor[i];

                                // Wrap UVs to 0-1 range
                                float wrapped_u = pixel_uv_u[i] - floorf(pixel_uv_u[i]);
                                float wrapped_v = 1.0f - (pixel_uv_v[i] - floorf(pixel_uv_v[i])); // Flip V coordinate for correct orientation

                                // Convert to texture coordinates
                                int texel_x = (int)(wrapped_u * diffuse_texture->width);
                                int texel_y = (int)(wrapped_v * diffuse_texture->height);

                                if (texel_x < 0) texel_x = 0;
                                if (texel_y < 0) texel_y = 0;
                                if (texel_x >= diffuse_texture->width) texel_x = diffuse_texture->width - 1;
                                if (texel_y >= diffuse_texture->height) texel_y = diffuse_texture->height - 1;

                                // Decode the texel (stb_image loads as RGBA in byte order)
                                uint32_t texel = diffuse_texture->pixels[texel_y * diffuse_texture->width + texel_x];
                                pixel_red[i]   = ((texel >> 0) & 0xFF) / 255.0f;
                                pixel_green[i] = ((texel >> 8) & 0xFF) / 255.0f;
                                pixel_blue[i]  = ((texel >> 16) & 0xFF) / 255.0f;
                            }
                            if (use_blinn_phong_shading) {
                                float normal_length = sqrtf(pixel_world_normal_x[i] * pixel_world_normal_x[i] + pixel_world_normal_y[i] * pixel_world_normal_y[i] + pixel_world_normal_z[i] * pixel_world_normal_z[i]);
                                if (normal_length > 0.0001f) {
                                    float inverse_normal_length = 1.0f / normal_length;
                                    shade_blinn_phong(
                                        &pixel_red[i], &pixel_green[i], &pixel_blue[i],
                                        pixel_red[i], pixel_green[i], pixel_blue[i],
                                        pixel_world_normal_x[i] * inverse_normal_length, pixel_world_normal_y[i] * inverse_normal_length, pixel_world_normal_z[i] * inverse_normal_length,
                                        pixel_world_position_x[i], pixel_world_position_y[i], pixel_world_position_z[i],
                                        light_direction_x, light_direction_y, light_direction_z,
                                        scene, triangle->material);
                                }
                            }
                            RGBVector3f pixel_color = { pixel_red[i], pixel_green[i], pixel_blue[i] };
                            app_context->frame_buffer[frame_buffer_index + i] = convert_rgbvec3f_to_hex_color(pixel_color);
                        }
                    }
                    weight_0_pixel_row += weight_0_step_y;
                    weight_1_pixel_row += weight_1_step_y;
                    weight_2_pixel_row += weight_2_step_y;
                }
            } else {
                // Partially inside, need to check each pixel
                float weight_0_pixel_row = weight_0_block;
                float weight_1_pixel_row = weight_1_block;
                float weight_2_pixel_row = weight_2_block;
                for (int y = block_y; y < block_y + block_size && y <= bounding_box_max_y; y++) {
                    __m128 weight_0_vector = _mm_set_ps(weight_0_pixel_row + 3 * weight_0_step_x, weight_0_pixel_row + 2 * weight_0_step_x, weight_0_pixel_row + weight_0_step_x, weight_0_pixel_row);
                    __m128 weight_1_vector = _mm_set_ps(weight_1_pixel_row + 3 * weight_1_step_x, weight_1_pixel_row + 2 * weight_1_step_x, weight_1_pixel_row + weight_1_step_x, weight_1_pixel_row);
                    __m128 weight_2_vector = _mm_set_ps(weight_2_pixel_row + 3 * weight_2_step_x, weight_2_pixel_row + 2 * weight_2_step_x, weight_2_pixel_row + weight_2_step_x, weight_2_pixel_row);

                    __m128 zero = _mm_setzero_ps();
                    __m128 inside_triangle_mask = _mm_and_ps(_mm_and_ps(_mm_cmple_ps(weight_0_vector, zero), _mm_cmple_ps(weight_1_vector, zero)), _mm_cmple_ps(weight_2_vector, zero));

                    __m128 barycentric_0 = _mm_mul_ps(weight_0_vector, inverse_triangle_area);
                    __m128 barycentric_1 = _mm_mul_ps(weight_1_vector, inverse_triangle_area);
                    __m128 barycentric_2 = _mm_mul_ps(weight_2_vector, inverse_triangle_area);

                    __m128 interpolated_depth = _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, _mm_set1_ps(triangle->depth_values[0])), _mm_mul_ps(barycentric_1, _mm_set1_ps(triangle->depth_values[1]))), _mm_mul_ps(barycentric_2, _mm_set1_ps(triangle->depth_values[2])));
                    int frame_buffer_index = y * app_context->window_resolution.x + block_x;
                    int combined_mask = _mm_movemask_ps(_mm_and_ps(inside_triangle_mask, _mm_cmplt_ps(interpolated_depth, _mm_loadu_ps(&app_context->depth_buffer->depth_values[frame_buffer_index]))));
                    float pixel_depths[4];
                    _mm_storeu_ps(pixel_depths, interpolated_depth);

                    float pixel_red[4], pixel_green[4], pixel_blue[4];
                    INTERPOLATE_ATTRIBUTE_3D(pixel_red, pixel_green, pixel_blue, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_color_red, vertex_0_color_green, vertex_0_color_blue,
                        vertex_1_color_red, vertex_1_color_green, vertex_1_color_blue,
                        vertex_2_color_red, vertex_2_color_green, vertex_2_color_blue);

                    float pixel_world_position_x[4], pixel_world_position_y[4], pixel_world_position_z[4];
                    float pixel_world_normal_x[4], pixel_world_normal_y[4], pixel_world_normal_z[4];
                    INTERPOLATE_ATTRIBUTE_3D(pixel_world_position_x, pixel_world_position_y, pixel_world_position_z, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_world_position_x, vertex_0_world_position_y, vertex_0_world_position_z,
                        vertex_1_world_position_x, vertex_1_world_position_y, vertex_1_world_position_z,
                        vertex_2_world_position_x, vertex_2_world_position_y, vertex_2_world_position_z);
                    INTERPOLATE_ATTRIBUTE_3D(pixel_world_normal_x, pixel_world_normal_y, pixel_world_normal_z, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_world_normal_x, vertex_0_world_normal_y, vertex_0_world_normal_z,
                        vertex_1_world_normal_x, vertex_1_world_normal_y, vertex_1_world_normal_z,
                        vertex_2_world_normal_x, vertex_2_world_normal_y, vertex_2_world_normal_z);

                    float pixel_uv_u[4], pixel_uv_v[4];
                    INTERPOLATE_ATTRIBUTE_2D(pixel_uv_u, pixel_uv_v, barycentric_0, barycentric_1, barycentric_2,
                        vertex_0_uv_u, vertex_0_uv_v,
                        vertex_1_uv_u, vertex_1_uv_v,
                        vertex_2_uv_u, vertex_2_uv_v);

                    float perspective_correction_factor[4];
                    _mm_storeu_ps(perspective_correction_factor, _mm_div_ps(_mm_set1_ps(1.0f), _mm_add_ps(_mm_add_ps(_mm_mul_ps(barycentric_0, vertex_0_one_over_perspective_w), _mm_mul_ps(barycentric_1, vertex_1_one_over_perspective_w)), _mm_mul_ps(barycentric_2, vertex_2_one_over_perspective_w))));

                    for (int i = 0; i < 4; i++) {
                        if ((combined_mask & (1 << i)) && (block_x + i <= bounding_box_max_x)) {
                            app_context->depth_buffer->depth_values[frame_buffer_index + i] = pixel_depths[i];
                            // If the material has a texture we sample it to get the effect it has on the diffuse color for this pixel
                            if (triangle->material && triangle->material->diffuse_texture) {
                                Texture *diffuse_texture = triangle->material->diffuse_texture;

                                // Recover the correct UV by dividing by the interpolated UV's perspective correction factor (which is 1/w interpolated across the triangle)
                                pixel_uv_u[i] *= perspective_correction_factor[i];
                                pixel_uv_v[i] *= perspective_correction_factor[i];

                                // Wrap UVs to 0-1 range
                                float wrapped_u = pixel_uv_u[i] - floorf(pixel_uv_u[i]);
                                float wrapped_v = 1.0f - (pixel_uv_v[i] - floorf(pixel_uv_v[i])); // Flip V coordinate for correct orientation

                                // Convert to texture coordinates
                                int texel_x = (int)(wrapped_u * diffuse_texture->width);
                                int texel_y = (int)(wrapped_v * diffuse_texture->height);

                                if (texel_x < 0) texel_x = 0;
                                if (texel_y < 0) texel_y = 0;
                                if (texel_x >= diffuse_texture->width) texel_x = diffuse_texture->width - 1;
                                if (texel_y >= diffuse_texture->height) texel_y = diffuse_texture->height - 1;

                                // Decode the texel (stb_image loads as RGBA in byte order)
                                uint32_t texel = diffuse_texture->pixels[texel_y * diffuse_texture->width + texel_x];
                                pixel_red[i]   = ((texel >> 0) & 0xFF) / 255.0f;
                                pixel_green[i] = ((texel >> 8) & 0xFF) / 255.0f;
                                pixel_blue[i]  = ((texel >> 16) & 0xFF) / 255.0f;
                            }
                            if (use_blinn_phong_shading) {
                                float normal_length = sqrtf(pixel_world_normal_x[i] * pixel_world_normal_x[i] + pixel_world_normal_y[i] * pixel_world_normal_y[i] + pixel_world_normal_z[i] * pixel_world_normal_z[i]);
                                if (normal_length > 0.0001f) {
                                    float inverse_normal_length = 1.0f / normal_length;
                                    shade_blinn_phong(
                                        &pixel_red[i], &pixel_green[i], &pixel_blue[i],
                                        pixel_red[i], pixel_green[i], pixel_blue[i],
                                        pixel_world_normal_x[i] * inverse_normal_length, pixel_world_normal_y[i] * inverse_normal_length, pixel_world_normal_z[i] * inverse_normal_length,
                                        pixel_world_position_x[i], pixel_world_position_y[i], pixel_world_position_z[i],
                                        light_direction_x, light_direction_y, light_direction_z,
                                        scene, triangle->material);
                                }
                            }
                            RGBVector3f pixel_color = { pixel_red[i], pixel_green[i], pixel_blue[i] };
                            app_context->frame_buffer[frame_buffer_index + i] = convert_rgbvec3f_to_hex_color(pixel_color);
                        }
                    }
                    weight_0_pixel_row += weight_0_step_y;
                    weight_1_pixel_row += weight_1_step_y;
                    weight_2_pixel_row += weight_2_step_y;
                }
            }
            weight_0_block += block_size * weight_0_step_x;
            weight_1_block += block_size * weight_1_step_x;
            weight_2_block += block_size * weight_2_step_x;
        }
        weight_0_block_row += block_size * weight_0_step_y;
        weight_1_block_row += block_size * weight_1_step_y;
        weight_2_block_row += block_size * weight_2_step_y;
    }
}

#undef INTERPOLATE_ATTRIBUTE_3D
#undef INTERPOLATE_ATTRIBUTE_2D

void rasterizer_worker(void *arg) {
    RasterizerJob *job = (RasterizerJob*)arg;
    for (int t = 0; t < job->triangle_count; t++) {
        RenderTriangle *triangle = job->render_triangles[t];
        fill_triangle(triangle, job->app_context, job->tile_x, job->tile_y, job->tile_width, job->tile_height, job->scene);
    }
    return;
}

void draw_debug_line_3d(AppContext *app_context, Scene *scene, Vector3f start, Vector3f end, RGBVector3f color) {
    // Transform to clip space
    Vector4f start_clip = mat4_multiply_vec4(scene->virtual_camera.perspective_projection_matrix, mat4_multiply_vec4(scene->virtual_camera.view_matrix, (Vector4f){start.x, start.y, start.z, 1.0f}));
    Vector4f end_clip = mat4_multiply_vec4(scene->virtual_camera.perspective_projection_matrix, mat4_multiply_vec4(scene->virtual_camera.view_matrix, (Vector4f){end.x, end.y, end.z, 1.0f}));

    // Skip if behind camera
    if (start_clip.w <= 0 || end_clip.w <= 0) return;

    // Perspective divide to NDC
    Vector2f start_ndc = { start_clip.x / start_clip.w, start_clip.y / start_clip.w };
    Vector2f end_ndc = { end_clip.x / end_clip.w, end_clip.y / end_clip.w };

    // NDC to screen
    Vector2i start_screen = convert_normalized_device_coordinates_to_screen_coordinates(start_ndc, app_context->window_resolution.x, app_context->window_resolution.y);
    Vector2i end_screen = convert_normalized_device_coordinates_to_screen_coordinates(end_ndc, app_context->window_resolution.x, app_context->window_resolution.y);

    draw_line_between_coordinates(app_context, start_screen.x, start_screen.y, end_screen.x, end_screen.y, color);
}

int render(AppContext *app_context, RenderList *render_list, Scene *scene) {
    bool texture_lock_result = SDL_LockTexture(app_context->texture, NULL, (void**)&app_context->frame_buffer, &(int){0});
    if (!texture_lock_result) {
        LOG_ERROR("Error locking SDL texture: %s", SDL_GetError());
        return 1;
    }

    clear_frame_buffer(app_context);

    int total_tiles = app_context->tiles_x * app_context->tiles_y;
    RenderTriangle ***tile_triangles = app_context->tile_triangles;
    int *tile_counts = app_context->tile_counts;
    int *tile_caps = app_context->tile_caps;
    memset(tile_counts, 0, total_tiles * sizeof(int));

    for (int t = 0; t < render_list->triangle_count; t++) {
        RenderTriangle *triangle = &render_list->triangles[t];
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

    int job_count = 0;
    for (int ty = 0; ty < app_context->tiles_y; ty++) {
        for (int tx = 0; tx < app_context->tiles_x; tx++) {
            int tile_index = ty * app_context->tiles_x + tx;
            if (tile_counts[tile_index] > 0) {
                RasterizerJob *job = &app_context->rasterizer_job_pool[job_count];
                job->app_context = app_context;
                job->scene = scene;
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

    thread_pool_wait_for_completion(&app_context->thread_pool);

    // Temporary to visualize which way the light direction is going
    Vector3f light_end = {0, 5, 0};  // point near the scene
    Vector3f light_origin = {
        light_end.x - scene->directional_light.direction.x * 3.0f,
        light_end.y - scene->directional_light.direction.y * 3.0f,
        light_end.z - scene->directional_light.direction.z * 3.0f
    };
    Vector3f mid = {
        (light_origin.x + light_end.x) * 0.5f,
        (light_origin.y + light_end.y) * 0.5f,
        (light_origin.z + light_end.z) * 0.5f
    };
    draw_debug_line_3d(app_context, scene, light_origin, mid, (RGBVector3f){1.0f, 1.0f, 0.0f});  // yellow = source
    draw_debug_line_3d(app_context, scene, mid, light_end, (RGBVector3f){1.0f, 0.0f, 0.0f});     // red = destination

    SDL_UnlockTexture(app_context->texture);
    SDL_RenderTexture(app_context->renderer, app_context->texture, NULL, NULL);
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

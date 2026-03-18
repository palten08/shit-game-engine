#include "../include/clipping.h"
#include "../include/types.h"

/**
 * @brief Enum representing the planes of the view frustum.
 */
enum FrustumPlane {
    LEFT_PLANE,
    RIGHT_PLANE,
    BOTTOM_PLANE,
    TOP_PLANE,
    NEAR_PLANE,
    FAR_PLANE
};

/**
 * @brief Checks if a vertex is inside a given frustum plane.
 * @param vertex The vertex to check.
 * @param plane The frustum plane to check against.
 * @return true if the vertex is inside the frustum plane, false otherwise.
 */
static bool is_vertex_inside_frustum_plane(Vector4f vertex, enum FrustumPlane plane) {
    switch (plane) {
        case LEFT_PLANE:
            // The vertex is inside the left plane if its x coordinate is greater than or equal to -w (the plane equation for the left plane is x = -w)
            return vertex.x >= -vertex.w;
        case RIGHT_PLANE:
            // The vertex is inside the right plane if its x coordinate is less than or equal to w (the plane equation for the right plane is x = w)
            return vertex.x <= vertex.w;
        case BOTTOM_PLANE:
            // The vertex is inside the bottom plane if its y coordinate is greater than or equal to -w (the plane equation for the bottom plane is y = -w)
            return vertex.y >= -vertex.w;
        case TOP_PLANE:
            // The vertex is inside the top plane if its y coordinate is less than or equal to w (the plane equation for the top plane is y = w)
            return vertex.y <= vertex.w;
        case NEAR_PLANE:
            // The vertex is inside the near plane if its z coordinate is greater than or equal to -w (the plane equation for the near plane is z = -w)
            return vertex.z >= -vertex.w;
        case FAR_PLANE:
            // The vertex is inside the far plane if its z coordinate is less than or equal to w (the plane equation for the far plane is z = w)
            return vertex.z <= vertex.w;
        default:
            return false; // Invalid plane
    }
}

/**
 * @brief Calculates the intersection point of a line segment defined by two vertices with a given frustum plane.
 * @param start_vertex The starting vertex of the line segment.
 * @param end_vertex The ending vertex of the line segment.
 * @param plane The frustum plane to calculate the intersection with.
 * @return The intersection point as a Vector4f.
 */
static Vector4f calculate_intersection(Vector4f start_vertex, Vector4f end_vertex, enum FrustumPlane plane, float *out_t) {
    // Calculate the direction vector from the start vertex to the end vertex
    Vector4f direction = {end_vertex.x - start_vertex.x, end_vertex.y - start_vertex.y, end_vertex.z - start_vertex.z, end_vertex.w - start_vertex.w};

    // Calculate the parameter t for the line-plane intersection
    float t;
    switch (plane) {
        case LEFT_PLANE:
            if (direction.x + direction.w == 0) {
                return start_vertex;
            }
            t = -(start_vertex.x + start_vertex.w) / (direction.x + direction.w);
            break;
        case RIGHT_PLANE:
            if (-(direction.x - direction.w) == 0) {
                return start_vertex;
            }
            t = (start_vertex.x - start_vertex.w) / -(direction.x - direction.w);
            break;
        case BOTTOM_PLANE:
            if (direction.y + direction.w == 0) {
                return start_vertex;
            }
            t = -(start_vertex.y + start_vertex.w) / (direction.y + direction.w);
            break;
        case TOP_PLANE:
            if (-(direction.y - direction.w) == 0) {
                return start_vertex;
            }
            t = (start_vertex.y - start_vertex.w) / -(direction.y - direction.w);
            break;
        case NEAR_PLANE:
            if (direction.z + direction.w == 0) {
                return start_vertex;
            }
            t = -(start_vertex.z + start_vertex.w) / (direction.z + direction.w);
            break;
        case FAR_PLANE:
            if (-(direction.z - direction.w) == 0) {
                return start_vertex;
            }
            t = (start_vertex.z - start_vertex.w) / -(direction.z - direction.w);
            break;
        default:
            return start_vertex; // Invalid plane
    }

    // Store the parameter t in the output variable
    if (out_t) {
        *out_t = t;
    }

    // Calculate the intersection point using the parameter t
    Vector4f intersection = {start_vertex.x + t * direction.x, start_vertex.y + t * direction.y, start_vertex.z + t * direction.z, start_vertex.w + t * direction.w};
    return intersection;
}

ClippingResult clip_triangle(Vector4f clip_space_vertices[3], RGBVector3f vertex_colors[3], Vector3f vertex_normals[3], Vector3f world_space_vertex_positions[3], Vector2f uv_coordinates[3]) {
    Vector4f vertex_index_buffer_a[9] = {clip_space_vertices[0], clip_space_vertices[1], clip_space_vertices[2]}; // Start with the original triangle vertices in the buffer
    Vector4f vertex_index_buffer_b[9]; // A second buffer to store intermediate results during clipping
    Vector4f *vertex_index_input = vertex_index_buffer_a; // Pointer to the current input buffer
    Vector4f *vertex_index_output = vertex_index_buffer_b; // Pointer to the current output buffer

    RGBVector3f vertex_color_buffer_a[9] = {vertex_colors[0], vertex_colors[1], vertex_colors[2]}; // Start with the original vertex colors in the buffer
    RGBVector3f vertex_color_buffer_b[9]; // A second buffer to store intermediate vertex colors during clipping
    RGBVector3f *vertex_color_input = vertex_color_buffer_a;
    RGBVector3f *vertex_color_output = vertex_color_buffer_b;

    Vector3f vertex_normal_buffer_a[9] = {vertex_normals[0], vertex_normals[1], vertex_normals[2]}; // Start with the original vertex normals in the buffer
    Vector3f vertex_normal_buffer_b[9]; // A second buffer to store intermediate vertex normals during clipping
    Vector3f *vertex_normal_input = vertex_normal_buffer_a;
    Vector3f *vertex_normal_output = vertex_normal_buffer_b;

    Vector3f world_space_vertex_position_buffer_a[9] = {world_space_vertex_positions[0], world_space_vertex_positions[1], world_space_vertex_positions[2]};
    Vector3f world_space_vertex_position_buffer_b[9];
    Vector3f *world_space_vertex_position_input = world_space_vertex_position_buffer_a;
    Vector3f *world_space_vertex_position_output = world_space_vertex_position_buffer_b;

    Vector2f uv_coordinate_buffer_a[9] = {uv_coordinates[0], uv_coordinates[1], uv_coordinates[2]};
    Vector2f uv_coordinate_buffer_b[9];
    Vector2f *uv_coordinate_input = uv_coordinate_buffer_a;
    Vector2f *uv_coordinate_output = uv_coordinate_buffer_b;

    int input_count = 3;

    int output_vertex_count = 0;
    for (int frustum_plane = LEFT_PLANE; frustum_plane <= FAR_PLANE; frustum_plane++) {
        for (int i = 0; i < input_count; i++) {
            int next_vertex_index = (i + 1) % input_count;
            if (is_vertex_inside_frustum_plane(vertex_index_input[i], frustum_plane) && is_vertex_inside_frustum_plane(vertex_index_input[next_vertex_index], frustum_plane)) {
                int idx = output_vertex_count;
                // Both vertices are inside the frustum plane, so we keep the next vertex
                vertex_index_output[idx] = (Vector4f){vertex_index_input[next_vertex_index].x, vertex_index_input[next_vertex_index].y, vertex_index_input[next_vertex_index].z, vertex_index_input[next_vertex_index].w};
                vertex_color_output[idx] = vertex_color_input[next_vertex_index];
                vertex_normal_output[idx] = vertex_normal_input[next_vertex_index];
                world_space_vertex_position_output[idx] = world_space_vertex_position_input[next_vertex_index];
                uv_coordinate_output[idx] = uv_coordinate_input[next_vertex_index];
                output_vertex_count++;
            } else if (is_vertex_inside_frustum_plane(vertex_index_input[i], frustum_plane) && !is_vertex_inside_frustum_plane(vertex_index_input[next_vertex_index], frustum_plane)) {
                float t;
                int idx = output_vertex_count;
                // Calculate an intersection
                Vector4f intersection = calculate_intersection(vertex_index_input[i], vertex_index_input[next_vertex_index], frustum_plane, &t);
                RGBVector3f resultant_color = {
                    vertex_color_input[i].r + (vertex_color_input[next_vertex_index].r - vertex_color_input[i].r) * t,
                    vertex_color_input[i].g + (vertex_color_input[next_vertex_index].g - vertex_color_input[i].g) * t,
                    vertex_color_input[i].b + (vertex_color_input[next_vertex_index].b - vertex_color_input[i].b) * t
                };
                Vector3f resultant_normal = {
                    vertex_normal_input[i].x + (vertex_normal_input[next_vertex_index].x - vertex_normal_input[i].x) * t,
                    vertex_normal_input[i].y + (vertex_normal_input[next_vertex_index].y - vertex_normal_input[i].y) * t,
                    vertex_normal_input[i].z + (vertex_normal_input[next_vertex_index].z - vertex_normal_input[i].z) * t
                };
                Vector3f resultant_world_space_vertex_position = {
                    world_space_vertex_position_input[i].x + (world_space_vertex_position_input[next_vertex_index].x - world_space_vertex_position_input[i].x) * t,
                    world_space_vertex_position_input[i].y + (world_space_vertex_position_input[next_vertex_index].y - world_space_vertex_position_input[i].y) * t,
                    world_space_vertex_position_input[i].z + (world_space_vertex_position_input[next_vertex_index].z - world_space_vertex_position_input[i].z) * t
                };
                Vector2f resultant_uv = {
                    uv_coordinate_input[i].x + (uv_coordinate_input[next_vertex_index].x - uv_coordinate_input[i].x) * t,
                    uv_coordinate_input[i].y + (uv_coordinate_input[next_vertex_index].y - uv_coordinate_input[i].y) * t
                };
                vertex_index_output[idx] = (Vector4f){intersection.x, intersection.y, intersection.z, intersection.w};
                vertex_color_output[idx] = resultant_color;
                vertex_normal_output[idx] = resultant_normal;
                world_space_vertex_position_output[idx] = resultant_world_space_vertex_position;
                uv_coordinate_output[idx] = resultant_uv;
                output_vertex_count++;
            } else if (!is_vertex_inside_frustum_plane(vertex_index_input[i], frustum_plane) && is_vertex_inside_frustum_plane(vertex_index_input[next_vertex_index], frustum_plane)) {
                float t;
                Vector4f intersection = calculate_intersection(vertex_index_input[i], vertex_index_input[next_vertex_index], frustum_plane, &t);
                RGBVector3f resultant_color = {
                    vertex_color_input[i].r + (vertex_color_input[next_vertex_index].r - vertex_color_input[i].r) * t,
                    vertex_color_input[i].g + (vertex_color_input[next_vertex_index].g - vertex_color_input[i].g) * t,
                    vertex_color_input[i].b + (vertex_color_input[next_vertex_index].b - vertex_color_input[i].b) * t
                };
                Vector3f resultant_normal = {
                    vertex_normal_input[i].x + (vertex_normal_input[next_vertex_index].x - vertex_normal_input[i].x) * t,
                    vertex_normal_input[i].y + (vertex_normal_input[next_vertex_index].y - vertex_normal_input[i].y) * t,
                    vertex_normal_input[i].z + (vertex_normal_input[next_vertex_index].z - vertex_normal_input[i].z) * t
                };
                Vector3f resultant_world_space_vertex_position = {
                    world_space_vertex_position_input[i].x + (world_space_vertex_position_input[next_vertex_index].x - world_space_vertex_position_input[i].x) * t,
                    world_space_vertex_position_input[i].y + (world_space_vertex_position_input[next_vertex_index].y - world_space_vertex_position_input[i].y) * t,
                    world_space_vertex_position_input[i].z + (world_space_vertex_position_input[next_vertex_index].z - world_space_vertex_position_input[i].z) * t
                };
                Vector2f resultant_uv = {
                    uv_coordinate_input[i].x + (uv_coordinate_input[next_vertex_index].x - uv_coordinate_input[i].x) * t,
                    uv_coordinate_input[i].y + (uv_coordinate_input[next_vertex_index].y - uv_coordinate_input[i].y) * t
                };
                int idx = output_vertex_count;
                vertex_index_output[idx] = (Vector4f){intersection.x, intersection.y, intersection.z, intersection.w};
                vertex_color_output[idx] = resultant_color;
                vertex_normal_output[idx] = resultant_normal;
                world_space_vertex_position_output[idx] = resultant_world_space_vertex_position;
                uv_coordinate_output[idx] = resultant_uv;
                output_vertex_count++;
                idx = output_vertex_count;
                vertex_index_output[idx] = (Vector4f){vertex_index_input[next_vertex_index].x, vertex_index_input[next_vertex_index].y, vertex_index_input[next_vertex_index].z, vertex_index_input[next_vertex_index].w};
                vertex_color_output[idx] = vertex_color_input[next_vertex_index];
                vertex_normal_output[idx] = vertex_normal_input[next_vertex_index];
                world_space_vertex_position_output[idx] = world_space_vertex_position_input[next_vertex_index];
                uv_coordinate_output[idx] = uv_coordinate_input[next_vertex_index];
                output_vertex_count++;
            } else {
                // Both vertices are outside the frustum plane, so we discard the edge
            }   
        }
        // Swap the input and output buffers for the next frustum plane
        Vector4f *temp = vertex_index_input;
        vertex_index_input = vertex_index_output;
        vertex_index_output = temp;
        RGBVector3f *temp_color = vertex_color_input;
        vertex_color_input = vertex_color_output;
        vertex_color_output = temp_color;
        Vector3f *temp_normal = vertex_normal_input;
        vertex_normal_input = vertex_normal_output;
        vertex_normal_output = temp_normal;
        Vector3f *temp_pos = world_space_vertex_position_input;
        world_space_vertex_position_input = world_space_vertex_position_output;
        world_space_vertex_position_output = temp_pos;
        Vector2f *temp_uv = uv_coordinate_input;
        uv_coordinate_input = uv_coordinate_output;
        uv_coordinate_output = temp_uv;
        input_count = output_vertex_count;
        output_vertex_count = 0;
        if (input_count == 0) {
            break; // The triangle is completely outside the view frustum, so we can stop clipping
        }
    }

    // The final clipped vertices are now in the input buffer
    ClippingResult result = {0};
    result.vertex_count = input_count;
    for (int i = 0; i < input_count; i++) {
        result.vertices[i] = vertex_index_input[i];
        result.vertex_colors[i] = vertex_color_input[i];
        result.vertex_normals[i] = vertex_normal_input[i];
        result.world_space_vertex_positions[i] = world_space_vertex_position_input[i];
        result.uv_coordinates[i] = uv_coordinate_input[i];
    }
    return result;
}

bool is_sphere_in_frustum(Vector4f center_view, float radius, float fov, float aspect, float near, float far) {
    float z = -center_view.z; // positive distance in front of camera

    // Behind camera or beyond far plane
    if (z + radius < near) return false;
    if (z - radius > far) return false;

    // Half extents at this depth
    float half_height = z * tanf(fov * 0.5f);
    float half_width = half_height * aspect;

    // Left/right
    if (center_view.x - radius > half_width) return false;
    if (center_view.x + radius < -half_width) return false;

    // Top/bottom
    if (center_view.y - radius > half_height) return false;
    if (center_view.y + radius < -half_height) return false;

    return true;
}
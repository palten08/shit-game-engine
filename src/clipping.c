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
static Vector4f calculate_intersection(Vector4f start_vertex, Vector4f end_vertex, enum FrustumPlane plane) {
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

    // Calculate the intersection point using the parameter t
    Vector4f intersection = {start_vertex.x + t * direction.x, start_vertex.y + t * direction.y, start_vertex.z + t * direction.z, start_vertex.w + t * direction.w};
    return intersection;
}

ClippingResult clip_triangle(Vector4f clip_space_vertices[3]) {
    Vector4f vertex_index_buffer_a[9] = {clip_space_vertices[0], clip_space_vertices[1], clip_space_vertices[2]}; // Start with the original triangle vertices in the buffer
    Vector4f vertex_index_buffer_b[9]; // A second buffer to store intermediate results during clipping
    Vector4f *vertex_index_input = vertex_index_buffer_a; // Pointer to the current input buffer
    Vector4f *vertex_index_output = vertex_index_buffer_b; // Pointer to the current output buffer
    int input_count = 3;

    int output_vertex_count = 0;
    for (int frustum_plane = LEFT_PLANE; frustum_plane <= FAR_PLANE; frustum_plane++) {
        for (int i = 0; i < input_count; i++) {
            int next_vertex_index = (i + 1) % input_count;
            if (is_vertex_inside_frustum_plane(vertex_index_input[i], frustum_plane) && is_vertex_inside_frustum_plane(vertex_index_input[next_vertex_index], frustum_plane)) {
                // Both vertices are inside the frustum plane, so we keep the next vertex
                vertex_index_output[output_vertex_count++] = (Vector4f){vertex_index_input[next_vertex_index].x, vertex_index_input[next_vertex_index].y, vertex_index_input[next_vertex_index].z, vertex_index_input[next_vertex_index].w};
            } else if (is_vertex_inside_frustum_plane(vertex_index_input[i], frustum_plane) && !is_vertex_inside_frustum_plane(vertex_index_input[next_vertex_index], frustum_plane)) {
                // Calculate an intersection
                Vector4f intersection = calculate_intersection(vertex_index_input[i], vertex_index_input[next_vertex_index], frustum_plane);
                vertex_index_output[output_vertex_count++] = (Vector4f){intersection.x, intersection.y, intersection.z, intersection.w};
            } else if (!is_vertex_inside_frustum_plane(vertex_index_input[i], frustum_plane) && is_vertex_inside_frustum_plane(vertex_index_input[next_vertex_index], frustum_plane)) {
                Vector4f intersection = calculate_intersection(vertex_index_input[i], vertex_index_input[next_vertex_index], frustum_plane);
                vertex_index_output[output_vertex_count++] = (Vector4f){intersection.x, intersection.y, intersection.z, intersection.w};
                vertex_index_output[output_vertex_count++] = (Vector4f){vertex_index_input[next_vertex_index].x, vertex_index_input[next_vertex_index].y, vertex_index_input[next_vertex_index].z, vertex_index_input[next_vertex_index].w};
            } else {
                // Both vertices are outside the frustum plane, so we discard the edge
            }   
        }
        // Swap the input and output buffers for the next frustum plane
        Vector4f *temp = vertex_index_input;
        vertex_index_input = vertex_index_output;
        vertex_index_output = temp;
        input_count = output_vertex_count;
        output_vertex_count = 0;
    }

    // The final clipped vertices are now in the input buffer
    ClippingResult result = {0};
    result.vertex_count = input_count;
    for (int i = 0; i < input_count; i++) {
        result.vertices[i] = vertex_index_input[i];
    }
    return result;
}
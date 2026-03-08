#include "../include/rendering_pipeline.h"

#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/vector_operations.h"
#include "../include/virtual_camera.h"
#include "../include/clipping.h"
#include "../include/coordinates.h"
#include "../include/app.h"
#include "../include/ecs.h"

RenderList generate_render_list(Scene *scene, AppContext *app_context) {
    RenderList generated_render_list = {0};
    Vector3f light_direction = vec3f_normalize((Vector3f){-1.0f, -1.0f, -1.0f});
    for (int i = 0; i < scene->registered_entity_count; i++) {
        Entity entity = i;
        TransformComponent *transform = get_component(scene, TRANSFORM, entity);
        MeshComponent *mesh = get_component(scene, MESH, entity);
        if (!transform || !mesh) {
            continue; // Skip entities that don't have both a transform and a mesh component
        }
        Mesh3D *mesh_data = &scene->asset_library.meshes[mesh->mesh_id];
        for (int t = 0; t < mesh_data->triangle_count; t++) {
            Vector4f clip_space_vertices[3];
            for (int v = 0; v < 3; v++) {
                Vector4f world_space_vertex = mat4_multiply_vec4(transform->model_matrix, (Vector4f){mesh_data->triangles[t].vertices[v].position.x, mesh_data->triangles[t].vertices[v].position.y, mesh_data->triangles[t].vertices[v].position.z, 1.0f});
                Vector4f view_space_vertex = mat4_multiply_vec4(scene->virtual_camera.view_matrix, world_space_vertex);
                clip_space_vertices[v] = mat4_multiply_vec4(scene->virtual_camera.perspective_projection_matrix, view_space_vertex);
            }

            // Transform face normals to world space
            // Only use the upper-left 3x3 portion of the model matrix for normal transformation
            Vector3f normal = mesh_data->face_normals[t];
            Vector3f world_space_normal = {
                transform->model_matrix.m[0][0] * normal.x + transform->model_matrix.m[0][1] * normal.y + transform->model_matrix.m[0][2] * normal.z,
                transform->model_matrix.m[1][0] * normal.x + transform->model_matrix.m[1][1] * normal.y + transform->model_matrix.m[1][2] * normal.z,
                transform->model_matrix.m[2][0] * normal.x + transform->model_matrix.m[2][1] * normal.y + transform->model_matrix.m[2][2] * normal.z
            };
            // Normalize the world space normal
            float length = sqrtf(world_space_normal.x * world_space_normal.x + world_space_normal.y * world_space_normal.y + world_space_normal.z * world_space_normal.z);
            if (length > 0.0f) {
                world_space_normal.x /= length;
                world_space_normal.y /= length;
                world_space_normal.z /= length;
            }
            // Brightness computation
            float ambient = 0.02f;
            float brightness = ambient + (1.0f - ambient) * fmaxf(0.0f, vec3f_dot_product(world_space_normal, vec3f_negate(light_direction)));
            if (brightness < 0.0f) {
                brightness = 0.0f; // Clamp brightness to zero to avoid negative values
            }
            // Apply brightness to the triangle color
            uint32_t color = mesh_data->triangles[t].color;
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;
            r = (uint8_t)(r * brightness);
            g = (uint8_t)(g * brightness);
            b = (uint8_t)(b * brightness);

            uint32_t shaded_color = 0xFF000000 | (r << 16) | (g << 8) | b;

            // Clip triangles here
            ClippingResult clipping_result = clip_triangle(clip_space_vertices);

            // Make new triangles out of the returned clipped vertices
            for (int c = 0; c < clipping_result.vertex_count - 2; c++) {
                RenderTriangle render_triangle;
                Vector2f ndc_coords_0 = convert_clip_space_to_normalized_device_coordinates(clipping_result.vertices[0]);
                Vector2f ndc_coords_1 = convert_clip_space_to_normalized_device_coordinates(clipping_result.vertices[c + 1]);
                Vector2f ndc_coords_2 = convert_clip_space_to_normalized_device_coordinates(clipping_result.vertices[c + 2]);
                render_triangle.screen_positions[0] = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords_0, app_context->window_resolution.x, app_context->window_resolution.y);
                render_triangle.screen_positions[1] = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords_1, app_context->window_resolution.x, app_context->window_resolution.y);
                render_triangle.screen_positions[2] = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords_2, app_context->window_resolution.x, app_context->window_resolution.y);
                render_triangle.depth_values[0] = clipping_result.vertices[0].z / clipping_result.vertices[0].w; // Perspective-correct depth value
                render_triangle.depth_values[1] = clipping_result.vertices[c + 1].z / clipping_result.vertices[c + 1].w;
                render_triangle.depth_values[2] = clipping_result.vertices[c + 2].z / clipping_result.vertices[c + 2].w;
                render_triangle.color = shaded_color;
                // Add the new triangle to the render list
                if (generated_render_list.triangle_count < MAX_RENDERABLE_TRIANGLES) {
                    generated_render_list.triangles[generated_render_list.triangle_count++] = render_triangle;
                }
            }
        }
    }
    return generated_render_list;
}

#include "../include/rendering_pipeline.h"

#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/virtual_camera.h"
#include "../include/clipping.h"
#include "../include/coordinates.h"
#include "../include/app.h"
#include "../include/ecs.h"

RenderList generate_render_list(Scene *scene, AppContext *app_context) {
    RenderList generated_render_list = {0};
    for (int i = 0; i < scene->registered_entity_count; i++) {
        Entity entity = i;
        TransformComponent *transform = get_component(scene, TRANSFORM, entity);
        MeshComponent *mesh = get_component(scene, MESH, entity);
        if (!transform || !mesh) {
            continue; // Skip entities that don't have both a transform and a mesh component
        }
        for (int t = 0; t < mesh->triangle_count; t++) {
            Vector4f clip_space_vertices[3];
            for (int v = 0; v < 3; v++) {
                Vector4f world_space_vertex = mat4_multiply_vec4(transform->model_matrix, (Vector4f){mesh->triangles[t].vertices[v].position.x, mesh->triangles[t].vertices[v].position.y, mesh->triangles[t].vertices[v].position.z, 1.0f});
                Vector4f view_space_vertex = mat4_multiply_vec4(scene->virtual_camera.view_matrix, world_space_vertex);
                clip_space_vertices[v] = mat4_multiply_vec4(scene->virtual_camera.perspective_projection_matrix, view_space_vertex);
            }

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
                render_triangle.color = mesh->triangles[t].color;
                // Add the new triangle to the render list
                if (generated_render_list.triangle_count < MAX_RENDERABLE_TRIANGLES) {
                    generated_render_list.triangles[generated_render_list.triangle_count++] = render_triangle;
                }
            }
        }
    }
    return generated_render_list;
}

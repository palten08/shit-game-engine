#include "../include/rendering_pipeline.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/vector_operations.h"
#include "../include/virtual_camera.h"
#include "../include/clipping.h"
#include "../include/coordinates.h"
#include "../include/app.h"
#include "../include/ecs.h"
#include "../include/multithreading.h"

void rendering_pipeline_worker(void *arg) {
    RenderingPipelineJob *job = (RenderingPipelineJob*)arg;
    int initial_triangle_capacity = 4096;
    job->render_triangles = malloc(initial_triangle_capacity * sizeof(RenderTriangle));
    if (!job->render_triangles) {
        fprintf(stderr, "Failed to allocate memory for render triangles\n");
        exit(EXIT_FAILURE);
    }

    Vector3f light_direction = job->scene->directional_light.direction;
    uint8_t light_r = (job->scene->directional_light.color >> 16) & 0xFF;
    uint8_t light_g = (job->scene->directional_light.color >> 8) & 0xFF;
    uint8_t light_b = job->scene->directional_light.color & 0xFF;

    int culled_entity_count = 0;

    for (int i = job->entity_start; i < job->entity_end; i++) {
        Entity entity = i;

        TransformComponent *transform = get_component(job->scene, TRANSFORM, entity);
        MeshComponent *mesh = get_component(job->scene, MESH, entity);
        if (!transform || !mesh) {
            continue;
        }

        Vector4f center_world = mat4_multiply_vec4(transform->model_matrix, (Vector4f){0, 0, 0, 1});
        Vector4f center_view = mat4_multiply_vec4(job->scene->virtual_camera.view_matrix, center_world);
        float radius = job->scene->asset_library.meshes[mesh->mesh_id].bounding_sphere_radius;
        if (!is_sphere_in_frustum(center_view, radius, job->scene->virtual_camera.field_of_view, job->scene->virtual_camera.aspect_ratio, job->scene->virtual_camera.near_plane, job->scene->virtual_camera.far_plane)) {
            culled_entity_count++;
            continue; // Skip this entity since it's not within the camera's view frustum
        }

        Mesh3D *mesh_data = &job->scene->asset_library.meshes[mesh->mesh_id];
        for (int t = 0; t < mesh_data->triangle_count; t++) {
            Vector4f clip_space_vertices[3];
            for (int v = 0; v < 3; v++) {
                Vector4f world_space_vertex = mat4_multiply_vec4(transform->model_matrix, (Vector4f){mesh_data->triangles[t].vertices[v].position.x, mesh_data->triangles[t].vertices[v].position.y, mesh_data->triangles[t].vertices[v].position.z, 1.0f});
                Vector4f view_space_vertex = mat4_multiply_vec4(job->scene->virtual_camera.view_matrix, world_space_vertex);
                clip_space_vertices[v] = mat4_multiply_vec4(job->scene->virtual_camera.perspective_projection_matrix, view_space_vertex);
            }

            // Only use the upper-left 3x3 portion of the model matrix for normal transformation
            Vector3f normal = mesh_data->face_normals[t];
            Vector3f world_space_normal = {
                transform->model_matrix.m[0][0] * normal.x + transform->model_matrix.m[0][1] * normal.y + transform->model_matrix.m[0][2] * normal.z,
                transform->model_matrix.m[1][0] * normal.x + transform->model_matrix.m[1][1] * normal.y + transform->model_matrix.m[1][2] * normal.z,
                transform->model_matrix.m[2][0] * normal.x + transform->model_matrix.m[2][1] * normal.y + transform->model_matrix.m[2][2] * normal.z
            };
            // Normalize the world space normal
            float length = sqrtf(world_space_normal.x * world_space_normal.x + world_space_normal.y * world_space_normal.y + world_space_normal.z * world_space_normal.z);
            if (length > 0) {
                world_space_normal.x /= length;
                world_space_normal.y /= length;
                world_space_normal.z /= length;
            }

            float diffuse = fmaxf(0.0f, vec3f_dot_product(world_space_normal, vec3f_negate(light_direction)));
            float brightness = job->scene->directional_light.ambient_intensity + (1.0f - job->scene->directional_light.ambient_intensity) * diffuse * job->scene->directional_light.intensity;
            brightness = fminf(1.0f, brightness);

            uint32_t color = mesh_data->triangles[t].color;
            uint8_t r = (uint8_t)(((color >> 16) & 0xFF) * brightness * (light_r / 255.0f));
            uint8_t g = (uint8_t)(((color >> 8) & 0xFF) * brightness * (light_g / 255.0f));
            uint8_t b = (uint8_t)((color & 0xFF) * brightness * (light_b / 255.0f));

            uint32_t shaded_color = 0xFF000000 | (r << 16) | (g << 8) | b;

            ClippingResult clipping_result = clip_triangle(clip_space_vertices);

            for (int c = 0; c < clipping_result.vertex_count - 2; c++) {
                RenderTriangle render_triangle;
                Vector2f ndc_coords_0 = convert_clip_space_to_normalized_device_coordinates(clipping_result.vertices[0]);
                Vector2f ndc_coords_1 = convert_clip_space_to_normalized_device_coordinates(clipping_result.vertices[c + 1]);
                Vector2f ndc_coords_2 = convert_clip_space_to_normalized_device_coordinates(clipping_result.vertices[c + 2]);
                render_triangle.screen_positions[0] = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords_0, job->app_context->window_resolution.x, job->app_context->window_resolution.y);
                render_triangle.screen_positions[1] = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords_1, job->app_context->window_resolution.x, job->app_context->window_resolution.y);
                render_triangle.screen_positions[2] = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords_2, job->app_context->window_resolution.x, job->app_context->window_resolution.y);
                render_triangle.depth_values[0] = clipping_result.vertices[0].z / clipping_result.vertices[0].w; // Perspective-correct depth value
                render_triangle.depth_values[1] = clipping_result.vertices[c + 1].z / clipping_result.vertices[c + 1].w;
                render_triangle.depth_values[2] = clipping_result.vertices[c + 2].z / clipping_result.vertices[c + 2].w;
                render_triangle.color = shaded_color;
                if (job->triangle_count >= initial_triangle_capacity) {
                    initial_triangle_capacity *= 2;
                    job->render_triangles = realloc(job->render_triangles, initial_triangle_capacity * sizeof(RenderTriangle));
                    if (!job->render_triangles) {
                        fprintf(stderr, "Failed to reallocate memory for render triangles\n");
                        exit(EXIT_FAILURE);
                    }
                }
                job->render_triangles[job->triangle_count++] = render_triangle;
            }
        }
    }
    return;
}

RenderList generate_render_list(Scene *scene, AppContext *app_context) {
    int job_count = app_context->thread_pool.thread_count * 2;
    int entities_per_job = (scene->registered_entity_count + job_count - 1) / job_count;
    RenderingPipelineJob *jobs[job_count];
    int actual_job_count = 0;
    for (int i = 0; i < job_count; i++) {
        int entity_start = i * entities_per_job;
        int entity_end = fmin(entity_start + entities_per_job, scene->registered_entity_count);
        if (entity_start >= entity_end) {
            break; // No more entities to process
        }


        jobs[actual_job_count] = malloc(sizeof(RenderingPipelineJob));
        jobs[actual_job_count]->scene = scene;
        jobs[actual_job_count]->app_context = app_context;
        jobs[actual_job_count]->render_triangles = NULL;
        jobs[actual_job_count]->entity_start = entity_start;
        jobs[actual_job_count]->entity_end = entity_end;
        jobs[actual_job_count]->triangle_count = 0;
        jobs[actual_job_count]->triangle_capacity = 0;
        submit_job_to_thread_pool(&app_context->thread_pool, rendering_pipeline_worker, jobs[actual_job_count]);
        actual_job_count++;

    }
    // Wait for all jobs to finish
    thread_pool_wait_for_completion(&app_context->thread_pool);

    // Combine results from all jobs into a single render list
    RenderList render_list = {0};
    for (int i = 0; i < actual_job_count; i++) {
        if (jobs[i]->render_triangles) {
            render_list.triangles = realloc(render_list.triangles, (render_list.triangle_count + jobs[i]->triangle_count) * sizeof(RenderTriangle));
            if (!render_list.triangles) {
                fprintf(stderr, "Failed to reallocate memory for render list triangles\n");
                exit(EXIT_FAILURE);
            }
            memcpy(&render_list.triangles[render_list.triangle_count], jobs[i]->render_triangles, jobs[i]->triangle_count * sizeof(RenderTriangle));
            render_list.triangle_count += jobs[i]->triangle_count;
        }
        free(jobs[i]->render_triangles);
        free(jobs[i]);
    }
    return render_list;
}

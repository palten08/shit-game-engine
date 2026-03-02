#include "../include/debug.h"
#include "../include/types.h"
#include "../include/matrix.h"
#include "../include/vector.h"
#include "../include/coordinates.h"

Scene test_update_scene(Scene *scene, VirtualCamera *virtual_camera, double delta_time) {
    for (int i = 0; i < scene->cube_count; i++) {
  
        // Move the cube around to prove the perspective
        scene->cubes[i].current_position.x = 90.0f * sin(SDL_GetTicks() / 2000.0f);
        scene->cubes[i].current_position.z = 90.0f + -100.0f * cos(SDL_GetTicks() / 2000.0f);

        // Translate the cube to wherever it's current_position says it is
        Matrix4 model_matrix = mat4_create_translation_matrix(scene->cubes[i].current_position.x, scene->cubes[i].current_position.y, scene->cubes[i].current_position.z);

        // Create a rotation matrix for the current cube's rotation angle
        Matrix4 rotation_x = mat4_create_rotation_x_matrix(scene->cubes[i].current_rotation_angle);
        Matrix4 rotation_y = mat4_create_rotation_y_matrix(scene->cubes[i].current_rotation_angle);
        Matrix4 rotation_z = mat4_create_rotation_z_matrix(scene->cubes[i].current_rotation_angle);
        Matrix4 rotation_matrix = mat4_multiply(rotation_z, mat4_multiply(rotation_y, rotation_x)); // Combine the x, y, and z rotation matrices into a single rotation matrix that I can apply to the vertex position in one step
    
        // Combining all of the matrices into one
        Matrix4 combined_model_matrix = mat4_multiply(model_matrix, rotation_matrix);

        // Increment the cube's rotation angle for the next frame
        scene->cubes[i].current_rotation_angle += 1.0f * delta_time;

        // Iterate through the cube's vertices and apply the combined model matrix to each vertex position
        // We use the originals and not the currents to avoid the weird compounding transform thing
        for (int j = 0; j < 8; j++) {

            // This gives us vertex positions in world space
            Vector4f resultant_vertex_pos = mat4_multiply_vec4(combined_model_matrix, (Vector4f){scene->cubes[i].original_vertices[j].x, scene->cubes[i].original_vertices[j].y, scene->cubes[i].original_vertices[j].z, 1.0f}); // Apply the combined transformation matrix to the vertex position

            scene->cubes[i].vertices[j].position.z = resultant_vertex_pos.z;

            // Something with the view matrix
            Vector4f view_space_vertex_pos = mat4_multiply_vec4(virtual_camera->view_matrix, resultant_vertex_pos);

            // This gives us vertex positions in clip space, which is what we need for the perspective division step to get to NDC and then screen space
            Vector4f projected_vertex_pos = mat4_multiply_vec4(virtual_camera->perspective_projection_matrix, view_space_vertex_pos);

            if (projected_vertex_pos.w <= virtual_camera->near_plane || projected_vertex_pos.w >= virtual_camera->far_plane) {
                scene->cubes[i].vertices[j].is_visible = false; // Mark the vertex as not visible if it's behind the camera
                continue; // Avoid division by zero in the perspective division step
            } else {
                scene->cubes[i].vertices[j].is_visible = true; // Mark the vertex as visible if it's in front of the camera
            }

            // This gives us vertex positions in NDC
            Vector2f ndc_coords = convert_clip_space_to_normalized_device_coordinates(projected_vertex_pos);

            // This finally gives us actual pixel coordinates
            Vector2i screen_coords = convert_normalized_device_coordinates_to_screen_coordinates(ndc_coords, 800, 600);

            // Update the cube's vertices with the screen pixel coordinates
            // current_position keeps track of where the cube is in 3D space, but the vertices are what we actually draw, so we need to update those with the new screen coordinates every frame
            scene->cubes[i].vertices[j].position.x = screen_coords.x;
            scene->cubes[i].vertices[j].position.y = screen_coords.y;
        }
    }

    return *scene;
}
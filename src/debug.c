#include "../include/debug.h"
#include "../include/types.h"
#include "../include/matrix.h"
#include "../include/vector.h"

Scene test_update_scene(Scene *scene) {
    for (int i = 0; i < scene->square_count; i++) {
        // Create a rotation matrix for the current square's rotation angle
        Matrix3 rotation_matrix = mat3_create_rotation_matrix(scene->squares[i].current_rotation_angle);
        // Increment the square's rotation angle for the next frame
        scene->squares[i].current_rotation_angle += 0.01f; // Adjust

        // Translation matrix used for moving the vertex position to the origin
        Matrix3 to_origin_matrix = mat3_create_translation_matrix(-scene->squares[i].origin.x, -scene->squares[i].origin.y);

        // Translation matrix used for moving the vertex back to where it was after rotating around the origin
        Matrix3 from_origin_matrix = mat3_create_translation_matrix(scene->squares[i].origin.x, scene->squares[i].origin.y);

        Matrix3 combined = mat3_multiply(from_origin_matrix, mat3_multiply(rotation_matrix, to_origin_matrix)); // Combine the translation and rotation matrices into a single matrix that I can apply to the vertex position in one step

        // Iterate through the square's vertices and apply the combined transformation matrix to each vertex position
        // We use the originals and not the currents to avoid the weird compounding transform thing
        for (int j = 0; j < 4; j++) {

            Vector3f resultant_vertex_pos = mat3_multiply_vec3(combined, (Vector3f){scene->squares[i].original_vertices[j].x, scene->squares[i].original_vertices[j].y, 1.0f}); // Apply the combined transformation matrix to the vertex position

            // Update the vertex position with the new rotated position
            scene->squares[i].vertices[j].position.x = (int)resultant_vertex_pos.x;
            scene->squares[i].vertices[j].position.y = (int)resultant_vertex_pos.y;
        }
    }

    for (int i = 0; i < scene->cube_count; i++) {
        
        // Create a rotation matrix for the current cube's rotation angle
        Matrix4 rotation_x = mat4_create_rotation_x_matrix(scene->cubes[i].current_rotation_angle);
        Matrix4 rotation_y = mat4_create_rotation_y_matrix(scene->cubes[i].current_rotation_angle);
        Matrix4 rotation_z = mat4_create_rotation_z_matrix(scene->cubes[i].current_rotation_angle);
        Matrix4 rotation_matrix = mat4_multiply(rotation_z, mat4_multiply(rotation_y, rotation_x)); // Combine the x, y, and z rotation matrices into a single rotation matrix that I can apply to the vertex position in one step
        
        // Increment the cube's rotation angle for the next frame
        scene->cubes[i].current_rotation_angle += 0.01f; // Adjust

        // Translation matrix used for moving the vertex position to the origin
        Matrix4 to_origin_matrix = mat4_create_translation_matrix(-scene->cubes[i].origin.x, -scene->cubes[i].origin.y, -scene->cubes[i].origin.z);

        // Translation matrix used for moving the vertex back to where it was after rotating around the origin
        Matrix4 from_origin_matrix = mat4_create_translation_matrix(scene->cubes[i].origin.x, scene->cubes[i].origin.y, scene->cubes[i].origin.z);

        Matrix4 combined = mat4_multiply(from_origin_matrix, mat4_multiply(rotation_matrix, to_origin_matrix)); // Combine the translation and rotation matrices into a single matrix that I can apply to the vertex position in one step

        // Iterate through the cube's vertices and apply the combined transformation matrix to each vertex position
        // We use the originals and not the currents to avoid the weird compounding transform thing
        for (int j = 0; j < 8; j++) {

            Vector4f resultant_vertex_pos = mat4_multiply_vec4(combined, (Vector4f){scene->cubes[i].original_vertices[j].x, scene->cubes[i].original_vertices[j].y, scene->cubes[i].original_vertices[j].z, 1.0f}); // Apply the combined transformation matrix to the vertex position

            // Update the vertex position with the new rotated position
            scene->cubes[i].vertices[j].position.x = (int)resultant_vertex_pos.x;
            scene->cubes[i].vertices[j].position.y = (int)resultant_vertex_pos.y;
            // We ignore the z value for now since we're doing a super simple orthographic projection where we just drop the z coordinate, but we could do some really simple fake depth stuff by using the z value to scale the x and y coordinates or something like that in the future
        }
    }

    return *scene;
}
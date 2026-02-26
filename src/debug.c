#include "../include/debug.h"
#include "../include/types.h"
#include "../include/matrix.h"
#include "../include/vector.h"

Scene test_update_scene(Scene *scene) {
    // Matrix used for slightly rotating on every frame
    Matrix3 rotation_matrix = mat3_create_rotation_matrix(0.01f);

    for (int i = 0; i < scene->square_count; i++) {
        // Translation matrix used for moving the vertex position to the origin
        Matrix3 to_origin_matrix = mat3_create_translation_matrix(-scene->squares[i].origin.x, -scene->squares[i].origin.y);

        // Translation matrix used for moving the vertex back to where it was after rotating around the origin
        Matrix3 from_origin_matrix = mat3_create_translation_matrix(scene->squares[i].origin.x, scene->squares[i].origin.y);

        Matrix3 combined = mat3_multiply(from_origin_matrix, mat3_multiply(rotation_matrix, to_origin_matrix)); // Combine the translation and rotation matrices into a single matrix that I can apply to the vertex position in one step

        // Iterate through the square's vertices and apply the combined transformation matrix to each vertex position
        for (int j = 0; j < 4; j++) {

            Vector3f resultant_vertex_pos = mat3_multiply_vec3(combined, (Vector3f){scene->squares[i].vertices[j].position.x, scene->squares[i].vertices[j].position.y, 1.0f}); // Apply the combined transformation matrix to the vertex position

            // Update the vertex position with the new rotated position
            scene->squares[i].vertices[j].position.x = (int)resultant_vertex_pos.x;
            scene->squares[i].vertices[j].position.y = (int)resultant_vertex_pos.y;
        }

        // I feel like this all I need but we'll see
    }

    return *scene;
}
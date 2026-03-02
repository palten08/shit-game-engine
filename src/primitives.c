#include "../include/primitives.h"
#include "../include/types.h"

SquareData2D create_2D_square(Vector2i starting_position, uint32_t color, int size) {
    // Starting position will represent the top left vertex
    // We're gonna build the rectangle by going clockwise
    SquareData2D new_square;
    new_square.vertices[0] = (VertexData2D){starting_position, color};
    new_square.vertices[1] = (VertexData2D){(Vector2i){starting_position.x + size, starting_position.y}, color};
    new_square.vertices[2] = (VertexData2D){(Vector2i){starting_position.x + size, starting_position.y + size}, color};
    new_square.vertices[3] = (VertexData2D){(Vector2i){starting_position.x, starting_position.y + size}, color};

    // Calculate the origin as being dead in the middle of the square, which will make rotation easier later on
    new_square.origin = (Vector2i){starting_position.x + size / 2, starting_position.y + size / 2};

    // Store the original vertex positions for resetting transformations later on if needed
    for (int i = 0; i < 4; i++) {
        new_square.original_vertices[i] = new_square.vertices[i].position;
    }

    return new_square;
}

CubeData3D create_3D_cube(uint32_t color, int size) {
    CubeData3D new_cube;
    float half_size = size / 2.0f;
    // Front face
    new_cube.vertices[0] = (VertexData3D){(Vector3f){-half_size, -half_size, -half_size}, color, true};
    new_cube.vertices[1] = (VertexData3D){(Vector3f){half_size, -half_size, -half_size}, color, true};
    new_cube.vertices[2] = (VertexData3D){(Vector3f){half_size, half_size, -half_size}, color, true};
    new_cube.vertices[3] = (VertexData3D){(Vector3f){-half_size, half_size, -half_size}, color, true};
    // Back face
    new_cube.vertices[4] = (VertexData3D){(Vector3f){-half_size, -half_size, half_size}, color, true};
    new_cube.vertices[5] = (VertexData3D){(Vector3f){half_size, -half_size, half_size}, color, true};
    new_cube.vertices[6] = (VertexData3D){(Vector3f){half_size, half_size, half_size}, color, true};
    new_cube.vertices[7] = (VertexData3D){(Vector3f){-half_size, half_size, half_size}, color, true};

    // Store the original vertex positions for resetting transformations later on if needed
    for (int i = 0; i < 8; i++) {
        new_cube.original_vertices[i] = new_cube.vertices[i].position;
    }

    new_cube.origin = (Vector3f){0.0f, 0.0f, 0.0f};
    // Set the current rotation angle to 0
    new_cube.current_rotation_angle = 0.0f;

    return new_cube;
}
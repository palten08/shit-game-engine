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

CubeData3D create_3D_cube(Vector3f starting_position, uint32_t color, int size) {
    CubeData3D new_cube;
    // Front face
    new_cube.vertices[0] = (VertexData3D){starting_position, color};
    new_cube.vertices[1] = (VertexData3D){(Vector3f){starting_position.x + size, starting_position.y, starting_position.z}, color};
    new_cube.vertices[2] = (VertexData3D){(Vector3f){starting_position.x + size, starting_position.y + size, starting_position.z}, color};
    new_cube.vertices[3] = (VertexData3D){(Vector3f){starting_position.x, starting_position.y + size, starting_position.z}, color};
    // Back face
    new_cube.vertices[4] = (VertexData3D){(Vector3f){starting_position.x, starting_position.y, starting_position.z + size}, color};
    new_cube.vertices[5] = (VertexData3D){(Vector3f){starting_position.x + size, starting_position.y, starting_position.z + size}, color};
    new_cube.vertices[6] = (VertexData3D){(Vector3f){starting_position.x + size, starting_position.y + size, starting_position.z + size}, color};
    new_cube.vertices[7] = (VertexData3D){(Vector3f){starting_position.x, starting_position.y + size, starting_position.z + size}, color};

    printf("Created cube with vertices:\n");
    for (int i = 0; i < 8; i++) {
        printf("Vertex %d: (%f, %f, %f)\n", i, new_cube.vertices[i].position.x, new_cube.vertices[i].position.y, new_cube.vertices[i].position.z);
    }

    // Store the original vertex positions for resetting transformations later on if needed
    for (int i = 0; i < 8; i++) {
        new_cube.original_vertices[i] = new_cube.vertices[i].position;
    }

    // Calculate the origin for the cube
    // Now taking into account the z-axis for the origin since this is a 3D cube, we want the origin to be in the middle of the cube in all three dimensions
    new_cube.origin = (Vector3f){starting_position.x + size / 2, starting_position.y + size / 2, starting_position.z + size / 2};
    // Set the current rotation angle to 0
    new_cube.current_rotation_angle = 0.0f;

    return new_cube;
}
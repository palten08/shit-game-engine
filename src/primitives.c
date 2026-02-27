#include "../include/primitives.h"
#include "../include/types.h"

SquareData2D create_2D_square(Vector2i starting_position, uint32_t color, int size) {
    // Starting position will represent the top left vertex
    // We're gonna build the rectangle by going clockwise
    SquareData2D new_square;
    new_square.vertices[0] = (VertexData){starting_position, color};
    new_square.vertices[1] = (VertexData){(Vector2i){starting_position.x + size, starting_position.y}, color};
    new_square.vertices[2] = (VertexData){(Vector2i){starting_position.x + size, starting_position.y + size}, color};
    new_square.vertices[3] = (VertexData){(Vector2i){starting_position.x, starting_position.y + size}, color};

    // Calculate the origin as being dead in the middle of the square, which will make rotation easier later on
    new_square.origin = (Vector2i){starting_position.x + size / 2, starting_position.y + size / 2};

    // Store the original vertex positions for resetting transformations later on if needed
    for (int i = 0; i < 4; i++) {
        new_square.original_vertices[i] = new_square.vertices[i].position;
    }

    return new_square;
}
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

    // Now make the edges to connect the vertices
    new_square.edges[0] = (LineData2D){new_square.vertices[0].position, new_square.vertices[1].position, color}; // Top edge
    new_square.edges[1] = (LineData2D){new_square.vertices[1].position, new_square.vertices[2].position, color}; // Right edge
    new_square.edges[2] = (LineData2D){new_square.vertices[2].position, new_square.vertices[3].position, color}; // Bottom edge
    new_square.edges[3] = (LineData2D){new_square.vertices[3].position, new_square.vertices[0].position, color}; // Left edge

    return new_square;
}
#include "../include/scene.h"
#include "../include/types.h"
#include "../include/primitives.h"

int add_square_to_scene(Scene *scene, SquareData2D square) {
    if (scene->square_count >= 256) {
        return -1; // Scene is full
    }
    scene->squares[scene->square_count] = square;
    scene->square_count++;
    return scene->square_count - 1; // Return the index of the added square
}

int add_line_to_scene(Scene *scene, LineData line) {
    if (scene->line_count >= 256) {
        return -1; // Scene is full
    }
    scene->lines[scene->line_count] = line;
    scene->line_count++;
    return scene->line_count - 1; // Return the index of the added line
}

int clear_scene(Scene *scene) {
    scene->square_count = 0;
    scene->line_count = 0;
    return 0; // Success
}

Scene create_test_scene(ScreenPositions_Unioned *screen_positions) {
    Scene scene = {0}; // Initialize all fields to zero

    // Square in the top left corner
    /** We need a function for generating a primitive SquareData2D based on a supplied starting point, origin and I guess color */
    SquareData2D top_left_square = create_2D_square(screen_positions->named.top_left, 0xFFFF0000, 20); // Red square
    add_square_to_scene(&scene, top_left_square);

    // Square in the top right corner
    SquareData2D top_right_square = create_2D_square(screen_positions->named.top_right, 0xFF00FF00, 30); // Green square
    add_square_to_scene(&scene, top_right_square);

    // Square in the bottom left corner
    SquareData2D bottom_left_square = create_2D_square(screen_positions->named.bottom_left, 0xFF0000FF, 40); // Blue square
    add_square_to_scene(&scene, bottom_left_square);

    // Square in the bottom right corner
    SquareData2D bottom_right_square = create_2D_square(screen_positions->named.bottom_right, 0xFFFFFF00, 50); // Yellow square
    add_square_to_scene(&scene, bottom_right_square);

    // Square in the absolute middle
    SquareData2D middle_square = create_2D_square(screen_positions->named.absolute_middle, 0xFFFF00FF, 60); // Magenta square
    add_square_to_scene(&scene, middle_square);
    
    // Add some test lines
    LineData line1 = {{200, 200}, {300, 300}, 0xFF0000FF}; // Blue line going diagonally down to the right
    LineData line2 = {{300, 200}, {200, 300}, 0xFFFFFF00}; // Yellow line going diagonally down to the left
    LineData line3 = {{250, 200}, {250, 300}, 0xFFFF0000}; // Red vertical line
    LineData line4 = {{200, 250}, {300, 250}, 0xFF00FF00}; // Green horizontal line
    add_line_to_scene(&scene, line1);
    add_line_to_scene(&scene, line2);
    add_line_to_scene(&scene, line3);
    add_line_to_scene(&scene, line4);
    
    return scene;
}
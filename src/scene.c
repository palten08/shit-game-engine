#include "../include/scene.h"
#include "../include/types.h"
#include "../include/primitives.h"

/**
 * @brief Adds a square to the scene.
 * @param scene Pointer to the scene.
 * @param square The square data to add.
 * @return The index of the added square, or -1 if the scene is full.
 */
int add_square_to_scene(Scene *scene, SquareData2D square) {
    if (scene->square_count >= 256) {
        return -1; // Scene is full
    }
    scene->squares[scene->square_count] = square;
    scene->square_count++;
    return scene->square_count - 1; // Return the index of the added square
}

/**
 * @brief Adds a line to the scene.
 * @param scene Pointer to the scene.
 * @param line The line data to add.
 * @return The index of the added line, or -1 if the scene is full.
 */
int add_line_to_scene(Scene *scene, LineData2D line) {
    if (scene->line_count >= 256) {
        return -1; // Scene is full
    }
    scene->lines[scene->line_count] = line;
    scene->line_count++;
    return scene->line_count - 1; // Return the index of the added line
}

/**
 * @brief Adds a cube to the scene.
 * @param scene Pointer to the scene.
 * @param cube The cube data to add.
 * @return The index of the added cube, or -1 if the scene is full.
 */
int add_cube_to_scene(Scene *scene, CubeData3D cube) {
    if (scene->cube_count >= 16) {
        return -1; // Scene is full
    }
    scene->cubes[scene->cube_count] = cube;
    scene->cube_count++;
    return scene->cube_count - 1; // Return the index of the added cube
}

/**
 * @brief Clears the scene by resetting the square, line, and cube counts.
 * @param scene Pointer to the scene.
 * @return 0 on success.
 */
int clear_scene(Scene *scene) {
    scene->square_count = 0;
    scene->line_count = 0;
    scene->cube_count = 0;
    return 0; // Success
}

/**
 * @brief Creates a test scene with some squares and lines for demonstration purposes.
 * @param screen_positions Pointer to the screen positions union, used to position the squares.
 * @return The created test scene.
 */
Scene create_test_scene(ScreenPositions_Unioned *screen_positions) {
    Scene scene = {0}; // Initialize all fields to zero

    // Red square in the top left corner
    // create_2D_square uses the point you supply as the top left
    // So we need to adjust some of these starting positions to ensure the squares are fully visible on the screen
    // It seems like for right now the top right, bottom right and bottom left are affected by this
    // There is probably an elegant solution for handling this but that's for later
    SquareData2D top_left_square = create_2D_square(screen_positions->named.top_left, 0xFFFF0000, 20);
    add_square_to_scene(&scene, top_left_square);

    // Green square in the top right corner
    // We have to shift this back to the left by the size of the square to ensure it is fully visible on the screen
    SquareData2D top_right_square = create_2D_square((Vector2i){screen_positions->named.top_right.x - 30, screen_positions->named.top_right.y}, 0xFF00FF00, 30);
    add_square_to_scene(&scene, top_right_square);

    // Blue square in the bottom left corner
    // This has to go up to be fully visible on the screen
    SquareData2D bottom_left_square = create_2D_square((Vector2i){screen_positions->named.bottom_left.x, screen_positions->named.bottom_left.y - 40}, 0xFF0000FF, 40);
    add_square_to_scene(&scene, bottom_left_square);

    // Yellow square in the bottom right corner
    // This has to go up and to the left to be fully visible on the screen
    SquareData2D bottom_right_square = create_2D_square((Vector2i){screen_positions->named.bottom_right.x - 50, screen_positions->named.bottom_right.y - 50}, 0xFFFFFF00, 50);
    add_square_to_scene(&scene, bottom_right_square);


    // Magenta square in the absolute middle
    // Technically has to move up and to the left by half the size of the square to be perfectly centered
    SquareData2D middle_square = create_2D_square((Vector2i){screen_positions->named.absolute_middle.x - 90, screen_positions->named.absolute_middle.y - 90}, 0xFFFF00FF, 60);
    add_square_to_scene(&scene, middle_square);
    
    
    // Add some test lines
    LineData2D line1 = {{200, 200}, {300, 300}, 0xFF0000FF}; // Blue line going diagonally down to the right
    LineData2D line2 = {{300, 200}, {200, 300}, 0xFFFFFF00}; // Yellow line going diagonally down to the left
    LineData2D line3 = {{250, 200}, {250, 300}, 0xFFFF0000}; // Red vertical line
    LineData2D line4 = {{200, 250}, {300, 250}, 0xFF00FF00}; // Green horizontal line
    add_line_to_scene(&scene, line1);
    add_line_to_scene(&scene, line2);
    add_line_to_scene(&scene, line3);
    add_line_to_scene(&scene, line4);

    
    // Test 3D cube right in the middle
    CubeData3D cube = create_3D_cube((Vector3f){screen_positions->named.absolute_middle.x - 20, screen_positions->named.absolute_middle.y - 20, 1}, 0xFF00FFFF, 40);
    add_cube_to_scene(&scene, cube);

    
    return scene;
}
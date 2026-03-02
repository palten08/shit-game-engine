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
 * @return The created test scene.
 */
Scene create_test_scene() {
    Scene scene = {0}; // Initialize all fields to zero

    CubeData3D cube = create_3D_cube(0xFF00FFFF, 90);
    cube.current_position = (Vector3f){0.0f, 0.0f, 150.0f};
    add_cube_to_scene(&scene, cube);

    return scene;
}
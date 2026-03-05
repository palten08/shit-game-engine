#include "../include/scene.h"
#include "../include/types.h"
#include "../include/primitives.h"
#include "../include/matrix_operations.h"
#include "../include/parson.h"

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

GameObject create_game_object(GameObjectType type, void *data, InitFunction on_init, UpdateFunction on_update, DestroyFunction on_destroy) {
    GameObject game_object = {0}; // Initialize all fields to zero
    game_object.type = type;
    game_object.data = data;
    game_object.on_init = on_init;
    game_object.on_update = on_update;
    game_object.on_destroy = on_destroy;
    return game_object;
}

int add_game_object_to_scene(Scene *scene, GameObject game_object) {
    if (scene->game_objects[255].type != 0) {
        return -1; // Scene is full
    }
    if (game_object.on_init) {
        game_object.on_init(game_object.data);
    }
    for (int i = 0; i < 256; i++) {
        if (scene->game_objects[i].type == 0) { // Find the first empty slot
            scene->game_objects[i] = game_object;
            return i; // Return the index of the added game object
        }
    }
    return -1; // Should never reach here
}

int add_child_game_object(GameObject *parent, GameObject child) {
    if (parent->child_count >= 16) {
        return -1; // Parent has maximum number of children
    }
    parent->children[parent->child_count] = malloc(sizeof(GameObject));
    if (!parent->children[parent->child_count]) {
        return -1; // Memory allocation failed
    }
    *(parent->children[parent->child_count]) = child;
    parent->child_count++;
    return 0; // Success
}

int unset_child_game_object(GameObject *parent, int index) {
    if (index < 0 || index >= parent->child_count) {
        return -1; // Invalid index
    }
    free(parent->children[index]);
    for (int i = index; i < parent->child_count - 1; i++) {
        parent->children[i] = parent->children[i + 1]; // Shift remaining children left
    }
    parent->children[parent->child_count - 1] = NULL; // Clear the last slot
    parent->child_count--;
    return 0; // Success
}

int destroy_game_object(GameObject *game_object) {
    if (game_object->on_destroy) {
        game_object->on_destroy(game_object->data);
    }
    for (int i = 0; i < game_object->child_count; i++) {
        destroy_game_object(game_object->children[i]);
        free(game_object->children[i]);
        game_object->children[i] = NULL;
    }
    game_object->child_count = 0;
    game_object->data = NULL;
    game_object->type = 0; // Reset type to indicate it's destroyed
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
    cube.model_matrix = mat4_create_translation_matrix(cube.current_position.x, cube.current_position.y, cube.current_position.z);
    add_cube_to_scene(&scene, cube);

    return scene;
}

Scene create_scene_from_file(const char *filename) {
    Scene scene = {0}; // Initialize all fields to zero
    JSON_Value *root_value = json_parse_file(filename);
    if (!root_value) {
        return scene; // Return empty scene if file cannot be parsed
    }
    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Array *game_objects_array = json_object_get_array(root_object, "game_objects");
    //char *serialized_string = json_serialize_to_string_pretty(root_value);
    //printf("Parsed JSON:\n%s\n", serialized_string);
    // TODO: Implement scene creation from JSON value
    for (int i = 0; i < 256; i++) {
        JSON_Object *game_object_json = json_array_get_object(game_objects_array, i);
        if (!game_object_json) {
            break; // No more game objects in the array
        }
        GameObject new_game_object = create_game_object(game_object_json, NULL, NULL, NULL, NULL);
        scene.game_objects[i] = new_game_object;
    }
    //json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return scene;
}
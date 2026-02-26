#include "../include/debug.h"
#include "../include/types.h"
#include "../include/vector.h"
#include "../include/matrix.h"


int test_update_scene(Scene *scene) {
    for (int i = 0; i < scene->square_count; i++) {
        // Make each square rotate around their own center

        // Convert each square's position into a vector3 so we can do rotation + transform using the w component

        // Multiply the square's vector 3 position against a rotation matrix to apply the rotation

        // Convert the resulting vector3 back into a vector2 and update the square's position with it
    }
}
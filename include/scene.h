#pragma once

#include "types.h"

int add_square_to_scene(Scene *scene, SquareData2D square);
int add_line_to_scene(Scene *scene, LineData2D line);
int add_cube_to_scene(Scene *scene, CubeData3D cube);
int clear_scene(Scene *scene);
Scene create_test_scene(ScreenPositions_Unioned *screen_positions);
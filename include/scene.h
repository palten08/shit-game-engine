#pragma once

#include "../include/types.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int add_square_to_scene(Scene *scene, SquareData2D square);
int add_line_to_scene(Scene *scene, LineData line);
int clear_scene(Scene *scene);
Scene create_test_scene(ScreenPositions_Unioned *screen_positions);
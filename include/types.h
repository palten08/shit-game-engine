#pragma once

typedef struct {
    int x;
    int y;
} Vector2i;

typedef struct {
    Vector2i top_left;
    Vector2i top_right;
    Vector2i bottom_left;
    Vector2i bottom_right;
    Vector2i top_middle;
    Vector2i bottom_middle;
    Vector2i absolute_middle;
} ScreenPositions;
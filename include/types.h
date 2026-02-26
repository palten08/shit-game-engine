#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#pragma once

// Double check alignment!


/** @struct Vector2i
 * @brief A 2D integer vector structure used for screen coordinates.
 */ 
typedef struct {
    /*@{*/
    int x; /**< The x-coordinate of the vector. (Horizontal component) | 4-bytes */
    int y; /**< The y-coordinate of the vector. (Vertical component) | 4-bytes */
    /*@{*/
} Vector2i;

/**
 * @brief A 3D floating-point vector structure used for mathematical operations, containing three 4-byte float members: x, y, and z.
*/
typedef struct {
    float x; /*!< The x-coordinate of the vector. (Horizontal component) | 4-bytes */
    float y; /*!< The y-coordinate of the vector. (Vertical component) | 4-bytes */
    float z; /*!< The z-coordinate of the vector. (Depth component) | 4-bytes */
} Vector3f;

/** A 4D floating-point vector structure used for mathematical operations, containing four 4-byte float members: x, y, z, and w. */
/** Members: */
/** - (float) x: The x-coordinate of the vector. (Horizontal component) */
/** - (float) y: The y-coordinate of the vector. (Vertical component) */
/** - (float) z: The z-coordinate of the vector. (Depth component) */
/** - (float) w: The w-coordinate of the vector. (Homogeneous component) */
typedef struct {
    float x; // 4-bytes
    float y; // 4-bytes
    float z; // 4-bytes
    float w; // 4-bytes
} Vector4f;

/** A structure representing the positions of key points on the screen, including corners, edges, and the center. All represented by 2D vectors containing their screen coordinates. */
/** Members: */
/** - (Vector2i) top_left: The top-left corner of the screen. */
/** - (Vector2i) top_right: The top-right corner of the screen. */
/** - (Vector2i) bottom_left: The bottom-left corner of the screen. */
/** - (Vector2i) bottom_right: The bottom-right corner of the screen. */
/** - (Vector2i) top_middle: The middle of the top edge of the screen. */
/** - (Vector2i) bottom_middle: The middle of the bottom edge of the screen. */
/** - (Vector2i) middle_left: The middle of the left edge of the screen. */
/** - (Vector2i) middle_right: The middle of the right edge of the screen. */
/** - (Vector2i) absolute_middle: The absolute center of the screen. */
typedef struct {
    /** The top-left corner of the screen. */
    Vector2i top_left; // 8-bytes
    /** The top-right corner of the screen. */
    Vector2i top_right; // 8-bytes
    /** The bottom-left corner of the screen. */
    Vector2i bottom_left; // 8-bytes
    /** The bottom-right corner of the screen. */
    Vector2i bottom_right; // 8-bytes
    /** The middle of the top edge of the screen. */
    Vector2i top_middle; // 8-bytes
    /** The middle of the bottom edge of the screen. */
    Vector2i bottom_middle; // 8-bytes
    /** The middle of the left edge of the screen. */
    Vector2i middle_left; // 8-bytes
    /** The middle of the right edge of the screen. */
    Vector2i middle_right; // 8-bytes
    /** The absolute center of the screen. */
    Vector2i absolute_middle; // 8-bytes
} ScreenPositions;

/** A union allowing access to screen positions either by name or as an array of vectors. */
/** Members: */
/** - (ScreenPositions) named: Access screen positions by their named fields. */
/** - (Vector2i[9]) array: Access screen positions as an array of vectors. The order of the array corresponds to the named fields in the ScreenPositions structure. */
typedef union ScreenPositions_Unioned {
    /** Access screen positions by their named fields. */
    ScreenPositions named;
    /** Access screen positions as an array of vectors. */
    Vector2i array[9];
} ScreenPositions_Unioned;

/** A structure representing the application context, including window, renderer, texture, and frame buffer information. */
/** Members: */
/** - (Vector2i) window_resolution: The resolution of the application window. */
/** - (SDL_Window*) window: The SDL window associated with the application. */
/** - (SDL_Renderer*) renderer: The SDL renderer associated with the application. */
/** - (SDL_Texture*) texture: The SDL texture used for rendering. */
/** - (uint32_t*) frame_buffer: The frame buffer used for rendering. */
/** - (bool) application_running: A flag indicating whether the application is running. */
typedef struct {
    /** The resolution of the application window. */
    Vector2i window_resolution; // 8-bytes
    /** The SDL window associated with the application. */
    SDL_Window *window; // 8-bytes
    /** The SDL renderer associated with the application. */
    SDL_Renderer *renderer; // 8-bytes
    /** The SDL texture used for rendering. */
    SDL_Texture *texture; // 8-bytes
    /** The frame buffer used for rendering. */
    uint32_t *frame_buffer; // 8-bytes
    /** A flag indicating whether the application is running. */
    bool application_running; // 1-byte
} AppContext;

/** 
 * @brief A structure representing a vertex, including its position and color.
 * @details This structure is used to define the properties of a vertex in 2D space
*/
typedef struct {
    Vector2i position; // 8-bytes
    uint32_t color; // 4-bytes
} VertexData;

/** A structure representing a line, including its start and end points and color. */
/** Members: */
/** - (Vector2i) start: The starting point of the line. */
/** - (Vector2i) end: The ending point of the line. */
/** - (uint32_t) color: The color of the line in ARGB format. */
typedef struct {
    /** The starting point of the line. */
    Vector2i start; // 8-bytes
    /** The ending point of the line. */
    Vector2i end; // 8-bytes
    /** The color of the line in ARGB format. */
    uint32_t color; // 4-bytes
} LineData2D;

/**
 * @brief A structure representing a 2D square, including its vertices, edges and origin point for transformations.
 * @details This structure defines a square in 2D space, consisting of an array of four vertices (each with a position and color) and an origin point that serves as the pivot for transformations such as rotation and scaling.
 */
typedef struct {
    /** An array of vertices representing the corners of the square. */
    /** This is a 2D square so I'm capping it at 4 */
    VertexData vertices[4]; // 4 vertices, 48 bytes
    /** The original vertex positions of the square, used for resetting transformations. */
    Vector2i original_vertices[4]; // 4 vertices, 48 bytes
    /** The origin point of the square, used for transformations. */
    Vector2i origin; // 8-bytes
    /** The rotation angle of the square, used for transformations. */
    float current_rotation_angle; // 4-bytes
} SquareData2D; // 120 bytes

/** A structure representing a scene, including its lines and squares. */
/** Members: */
/** - (LineData2D[256]) lines: The lines in the scene. */
/** - (SquareData2D[256]) squares: The squares in the scene. */
/** - (int) square_count: The number of squares in the scene. */
/** - (int) line_count: The number of lines in the scene. */
typedef struct {
    /** The squares in the scene. */
    SquareData2D squares[256]; // 30,720 bytes (or 30 kB)
    /** The lines in the scene. */
    LineData2D lines[256]; // 5120-bytes
    /** The number of squares in the scene. */
    int square_count; // 4-bytes
    /** The number of lines in the scene. */
    int line_count; // 4-bytes
} Scene;

/** A 4x4 matrix structure used for transformations in 3D space. */
/** Members: */
/** - (float[4][4]) m: The elements of the matrix, stored in row-major order. */
typedef struct {
    float m[4][4]; // 16 floats, 64 bytes
} Matrix4;

/** A 2x2 matrix structure used for transformations in 2D space. */
/** Members: */
/** - (float[2][2]) m: The elements of the matrix, stored in row-major order. */
typedef struct {
    float m[2][2]; // 4 floats, 16 bytes
} Matrix2;

/** A 3x3 matrix structure used for transformations in 2D space. */
/** Members: */
/** - (float[3][3]) m: The elements of the matrix, stored in row-major order. */
typedef struct {
    float m[3][3]; // 9 floats, 36 bytes
} Matrix3;
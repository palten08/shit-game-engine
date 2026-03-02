#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#pragma once

// Double check alignment!

/** @defgroup Vectors Vector Structures
 *  @{
 */

/** @struct Vector2i
 *
 * @brief A 2D integer vector structure used for screen coordinates.
 * 
 * 8 bytes
 */ 
typedef struct {
    /*@{*/
    int x; /**< The x-coordinate of the vector. (Horizontal component) | 4-bytes */
    int y; /**< The y-coordinate of the vector. (Vertical component) | 4-bytes */
    /*@{*/
} Vector2i;

/**
 * @brief A 2D floating-point vector structure used for mathematical operations, containing two 4-byte float members: x and y.
 * 
 * 8 bytes
*/
typedef struct {
    float x; // 4-bytes
    float y; // 4-bytes
} Vector2f;

/**
 * @brief A 3D floating-point vector structure used for mathematical operations, containing three 4-byte float members: x, y, and z.
 * 
 * 12 bytes
*/
typedef struct {
    float x; /*!< The x-coordinate of the vector. (Horizontal component) | 4-bytes */
    float y; /*!< The y-coordinate of the vector. (Vertical component) | 4-bytes */
    float z; /*!< The z-coordinate of the vector. (Depth component) | 4-bytes */
} Vector3f;

/**
 * @brief A 4D floating-point vector structure used for mathematical operations, containing four 4-byte float members: x, y, z, and w.
 * The w component is used for homogeneous coordinates, allowing for more complex transformations in 3D
 * 
 * 16 bytes
 */
typedef struct {
    float x; // 4-bytes
    float y; // 4-bytes
    float z; // 4-bytes
    float w; // 4-bytes
} Vector4f;

/** @} */ // End of Vectors group

/**
 * @brief A structure representing the positions of key points on the screen, including corners, edges, and the center. All represented by 2D vectors containing their screen coordinates.
 * 
 * 72 bytes
 */
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

/**
 * @brief A union allowing access to screen positions either by name or as an array of vectors.
 * 
 * 72 bytes (same as ScreenPositions, since it's a union)
 */
typedef union ScreenPositions_Unioned {
    /** Access screen positions by their named fields. */
    ScreenPositions named;
    /** Access screen positions as an array of vectors. */
    Vector2i array[9];
} ScreenPositions_Unioned;

/**
 * @brief A structure representing the application context, including window, renderer, texture, and frame buffer information.
 * 
 * 41 bytes
 */
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
 * This structure is used to define the properties of a vertex in 2D space
 * 
 * 12 bytes
*/
typedef struct {
    Vector2i position; // 8-bytes
    uint32_t color; // 4-bytes
    bool is_visible; // 1 byte
} VertexData2D;

/**
 * @brief A structure representing a vertex, including its position and color.
 * This structure is used to define the properties of a vertex in 3D space
 * 
 * 16 bytes
 */
typedef struct {
    Vector3f position; // 12-bytes
    uint32_t color; // 4-bytes
    bool is_visible; // 1 byte
} VertexData3D;

/**
 * @brief A structure representing a line, including its start and end points and color.
 * 
 * 20 bytes
 */
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
 * 
 * 92 bytes
 */
typedef struct {
    VertexData2D vertices[4]; // 48 bytes
    Vector2i original_vertices[4]; // 32 bytes
    Vector2i origin; // 8-bytes
    float current_rotation_angle; // 4-bytes
} SquareData2D;

typedef struct {
    int vertex_indices[3]; // 12 bytes
    uint32_t color; // 4-bytes
    bool is_visible; // 1 byte
} TriangleData3D;

typedef struct {
    TriangleData3D triangles[2]; // 24 bytes
    bool is_visible; // 1 byte
} QuadData3D;

/**
 * @brief A structure representing a 3D cube, including its vertices.
 * 
 * 208 bytes
 */
typedef struct {
    QuadData3D faces[6]; // 144 bytes
    VertexData3D vertices[8]; // 128 bytes
    Vector3f original_vertices[8]; // 96 bytes
    Vector3f origin; // 12 bytes
    Vector3f current_position; // 12 bytes
    float current_rotation_angle; // 4-bytes
} CubeData3D;

/**
 * @brief A structure representing a scene, including its lines and squares.
 * 
 * 32008 bytes
 */
typedef struct {
    SquareData2D squares[256]; // 23552 bytes
    LineData2D lines[256]; // 5120-bytes
    CubeData3D cubes[16]; // 3328 bytes
    int square_count; // 4-bytes
    int line_count; // 4-bytes
    int cube_count; // 4-bytes
} Scene;

/**
 * @brief A 4x4 matrix structure used for transformations in 3D space.
 * 
 * 64 bytes
 */
typedef struct {
    float m[4][4]; // 16 floats, 64 bytes
} Matrix4;

/**
 * @brief A 2x2 matrix structure used for transformations in 2D space.
 * 
 * 16 bytes
 */
typedef struct {
    float m[2][2]; // 4 floats, 16 bytes
} Matrix2;

/**
 * @brief A 3x3 matrix structure used for transformations in 2D space with homogeneous coordinates.
 * 
 * 36 bytes
 */
typedef struct {
    float m[3][3]; // 9 floats, 36 bytes
} Matrix3;

/**
 * @brief A structure representing a virtual camera in 3D space, including its field of view, aspect ratio, and near and far clipping planes.
 * 
 * 80 bytes
 */
typedef struct {
    Matrix4 view_matrix; // 64 bytes
    Matrix4 perspective_projection_matrix; // 64 bytes
    Vector3f current_position; // 12 bytes
    float field_of_view; // 4-bytes
    float aspect_ratio; // 4-bytes
    float near_plane; // 4-bytes
    float far_plane; // 4-bytes
} VirtualCamera;
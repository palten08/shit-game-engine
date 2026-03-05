#include "../include/virtual_camera.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include <math.h>

/**
 * @brief Initializes a virtual camera, and also creates a perspective projection matrix to accompany it
 * 
 * @param resolution_x The horizontal resolution of the window, used to calculate the camera's aspect ratio.
 * @param resolution_y The vertical resolution of the window, used to calculate the camera's aspect ratio.
 * @param field_of_view The field of view of the camera in degrees.
 * @param near_plane_distance The distance to the near clipping plane.
 * @param far_plane_distance The distance to the far clipping plane.
 * @return A initialized VirtualCamera structure.
 */
VirtualCamera initialize_virtual_camera(int resolution_x, int resolution_y, float field_of_view, float near_plane_distance, float far_plane_distance, Vector3f initial_position) {
    VirtualCamera camera;
    camera.field_of_view = field_of_view * (atan(1)*4 / 180.0f); // Convert from degrees to radians
    camera.aspect_ratio = (float)resolution_x / (float)resolution_y;
    camera.near_plane = near_plane_distance;
    camera.far_plane = far_plane_distance;
    camera.current_position = initial_position;
    camera.view_matrix = mat4_create_translation_matrix(-initial_position.x, -initial_position.y, -initial_position.z); // Create a view matrix that translates the world in the opposite direction of the camera's position
    camera.perspective_projection_matrix = mat4_create_perspective_projection_matrix(camera.field_of_view, camera.aspect_ratio, camera.near_plane, camera.far_plane);
    return camera;
}
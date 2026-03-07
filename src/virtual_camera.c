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
VirtualCamera initialize_virtual_camera(float aspect_ratio, float field_of_view, float near_plane_distance, float far_plane_distance, Vector3f initial_position, Vector3f initial_rotation, Vector3f look_target) {
    VirtualCamera camera;
    camera.field_of_view = field_of_view * (atan(1)*4 / 180.0f); // Convert from degrees to radians
    camera.aspect_ratio = aspect_ratio;
    camera.near_plane = near_plane_distance;
    camera.far_plane = far_plane_distance;
    camera.position = initial_position;
    camera.rotation = initial_rotation;
    camera.look_target = look_target;
    // Create the view matrix based on the camera's position and rotation
    camera.view_matrix = mat4_create_look_at_matrix(camera.position, camera.look_target, (Vector3f){0.0f, 1.0f, 0.0f});
    camera.perspective_projection_matrix = mat4_create_perspective_projection_matrix(camera.field_of_view, camera.aspect_ratio, camera.near_plane, camera.far_plane);
    return camera;
}
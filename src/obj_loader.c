#include <stdio.h>

#include "../include/obj_loader.h"
#include "../include/types.h"
#include "../include/vector_operations.h"
#include "../include/logging.h"

Mesh3D load_obj(const char *file_name) {
    LOG_DEBUG("Loading OBJ file: %s", file_name);
    Mesh3D loaded_mesh = {0};
    Point3D *loaded_vertices = malloc(sizeof(Point3D));
    Triangle3D *loaded_triangles = malloc(sizeof(Triangle3D));
    Vector3f *loaded_vertex_normals = malloc(sizeof(Vector3f));
    char file_data[256];
    FILE *file_pointer;
    file_pointer = fopen(file_name, "r");
    if (!file_pointer) {
        LOG_ERROR("Error: Failed to open OBJ file: %s", file_name);
        return loaded_mesh;
    }
    while (fgets(file_data, sizeof(file_data), file_pointer) != NULL) {
        char* file_data_copy = strdup(file_data); // Create a copy of the line for tokenization
        char* line_token = strtok(file_data_copy, " ");
        if (line_token == NULL) {
            free(file_data_copy);
            continue;
        }
        if (strcmp(line_token, "o") == 0) {
            sscanf(file_data, "o %s", loaded_mesh.name);
        } else if (strcmp(line_token, "v") == 0) {
            sscanf(file_data, "v %f %f %f", &loaded_vertices[loaded_mesh.vertex_count].position.x, &loaded_vertices[loaded_mesh.vertex_count].position.y, &loaded_vertices[loaded_mesh.vertex_count].position.z);
            loaded_mesh.vertex_count++;
            loaded_vertices = realloc(loaded_vertices, sizeof(Point3D) * (loaded_mesh.vertex_count + 1));
        } else if (strcmp(line_token, "vn") == 0) {
            sscanf(file_data, "vn %f %f %f", &loaded_vertex_normals[loaded_mesh.vertex_normal_count].x, &loaded_vertex_normals[loaded_mesh.vertex_normal_count].y, &loaded_vertex_normals[loaded_mesh.vertex_normal_count].z);
            loaded_mesh.vertex_normal_count++;
            loaded_vertex_normals = realloc(loaded_vertex_normals, sizeof(Vector3f) * (loaded_mesh.vertex_normal_count + 1));
        } else if (strcmp(line_token, "f") == 0) {
            // Check if we get 4 (sscanf will have returned 8) faces or 3 (sscanf will have returned 6) faces
            int temp_vertex_indices[4];
            int temp_vertex_normal_indices[4];
            int matches = sscanf(file_data, "f %d/%*d/%d %d/%*d/%d %d/%*d/%d %d/%*d/%d", &temp_vertex_indices[0], &temp_vertex_normal_indices[0], &temp_vertex_indices[1], &temp_vertex_normal_indices[1], &temp_vertex_indices[2], &temp_vertex_normal_indices[2], &temp_vertex_indices[3], &temp_vertex_normal_indices[3]);
            if (matches == 6) {
                // We have a triangle
                loaded_mesh.triangle_count++;
                loaded_triangles = realloc(loaded_triangles, sizeof(Triangle3D) * (loaded_mesh.triangle_count + 1));
                for (int i = 0; i < 3; i++) {
                    loaded_triangles[loaded_mesh.triangle_count - 1].vertex_indices[i] = temp_vertex_indices[i];
                    loaded_triangles[loaded_mesh.triangle_count - 1].vertex_normal_indices[i] = temp_vertex_normal_indices[i];
                }
            } else if (matches == 8) {
                // We have a quad, so we need to create 2 triangles

                // Wind 0 1 2
                loaded_mesh.triangle_count++;
                loaded_triangles = realloc(loaded_triangles, sizeof(Triangle3D) * (loaded_mesh.triangle_count + 1));
                for (int i = 0; i < 3; i++) {
                    loaded_triangles[loaded_mesh.triangle_count - 1].vertex_indices[i] = temp_vertex_indices[i];
                    loaded_triangles[loaded_mesh.triangle_count - 1].vertex_normal_indices[i] = temp_vertex_normal_indices[i];
                }
                // Wind 0 2 3
                loaded_mesh.triangle_count++;
                loaded_triangles = realloc(loaded_triangles, sizeof(Triangle3D) * (loaded_mesh.triangle_count + 1));
                loaded_triangles[loaded_mesh.triangle_count - 1].vertex_indices[0] = temp_vertex_indices[0];
                loaded_triangles[loaded_mesh.triangle_count - 1].vertex_normal_indices[0] = temp_vertex_normal_indices[0];
                loaded_triangles[loaded_mesh.triangle_count - 1].vertex_indices[1] = temp_vertex_indices[2];
                loaded_triangles[loaded_mesh.triangle_count - 1].vertex_normal_indices[1] = temp_vertex_normal_indices[2];
                loaded_triangles[loaded_mesh.triangle_count - 1].vertex_indices[2] = temp_vertex_indices[3];
                loaded_triangles[loaded_mesh.triangle_count - 1].vertex_normal_indices[2] = temp_vertex_normal_indices[3];
            } else {
                LOG_WARNING("Warning: Failed to parse face data from line: %s", file_data);
            }
        } else {
        }
       free(file_data_copy);
    }
    LOG_DEBUG("Finished reading OBJ file");
    fclose(file_pointer);

    // Get all the loaded data into the mesh structure
    loaded_mesh.triangles = malloc(sizeof(Triangle3D) * loaded_mesh.triangle_count);
    loaded_mesh.face_normals = malloc(sizeof(Vector3f) * loaded_mesh.triangle_count);
    loaded_mesh.vertex_normals = malloc(sizeof(Vector3f) * loaded_mesh.vertex_normal_count);
    for (int v = 0; v < loaded_mesh.vertex_normal_count; v++) {
        loaded_mesh.vertex_normals[v] = loaded_vertex_normals[v];
    }
    for (int t = 0; t < loaded_mesh.triangle_count; t++) {
        for (int i = 0; i < 3; i++) {
            float distance = vec3f_get_length(loaded_vertices[loaded_triangles[t].vertex_indices[i] - 1].position);
            if (distance > loaded_mesh.bounding_sphere_radius) {
                loaded_mesh.bounding_sphere_radius = distance;
            }
            int vertex_index = loaded_triangles[t].vertex_indices[i] - 1;
            int normal_index = loaded_triangles[t].vertex_normal_indices[i] - 1;
            if (vertex_index >= 0 && vertex_index < loaded_mesh.vertex_count) {
                loaded_triangles[t].vertices[i] = loaded_vertices[vertex_index ];
            }
            if (normal_index >= 0 && normal_index < loaded_mesh.vertex_normal_count) {
                loaded_triangles[t].vertices[i].normal = loaded_vertex_normals[normal_index];
            }
        }
        loaded_triangles[t].face_normal = vec3f_normalize(vec3f_cross_product(vec3f_subtract(loaded_triangles[t].vertices[1].position, loaded_triangles[t].vertices[0].position), vec3f_subtract(loaded_triangles[t].vertices[2].position, loaded_triangles[t].vertices[0].position)));
        loaded_triangles[t].color = 0xFFFFFFFF; // Default to white color for all triangles
        loaded_mesh.triangles[t] = loaded_triangles[t];
        loaded_mesh.face_normals[t] = loaded_triangles[t].face_normal;
    }
    free(loaded_vertices);
    free(loaded_vertex_normals);
    free(loaded_triangles);
    return loaded_mesh;
}
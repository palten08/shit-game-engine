#include "../include/scene.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/parson.h"
#include "../include/ecs.h"

Scene initialize_scene() {
    Scene scene = {0};
    scene.entity_manager.next_id = 0; // Start entity IDs from 0
    // Register built-in engine ECS components
    TRANSFORM = register_component(&scene, sizeof(TransformComponent));
    MESH = register_component(&scene, sizeof(MeshComponent));
    return scene;
}

Scene *load_scene_from_file(Scene *scene, const char *filename) {
    JSON_Value *root_value = json_parse_file(filename);
    if (!root_value) {
        return scene; // Return empty scene if file cannot be parsed
    }
    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Array *entities_array = json_object_get_array(root_object, "entities");
    // TODO: Implement scene creation from JSON value
    for (int i = 0; i < 256; i++) {
        JSON_Object *entity_json = json_array_get_object(entities_array, i);
        if (!entity_json) {
            break; // No more entities in the array
        }
        Entity entity_id = register_entity(scene);
        JSON_Object *components = json_object_get_object(entity_json, "components");
        JSON_Object *transform_json = json_object_get_object(components, "transform");
        if (transform_json) {
            JSON_Object *position_json = json_object_get_object(transform_json, "position");
            JSON_Object *rotation_json = json_object_get_object(transform_json, "rotation");
            JSON_Object *scale_json = json_object_get_object(transform_json, "scale");

            scene->component_masks[entity_id] |= (1ULL << TRANSFORM); // Mark that this entity has a transform component
            TransformComponent *transform_component = get_component(scene, TRANSFORM, entity_id);
            if (transform_component) {
                if (position_json) {
                    transform_component->position.x = json_object_get_number(position_json, "x");
                    transform_component->position.y = json_object_get_number(position_json, "y");
                    transform_component->position.z = json_object_get_number(position_json, "z");
                }
                if (rotation_json) {
                    transform_component->rotation.x = json_object_get_number(rotation_json, "x");
                    transform_component->rotation.y = json_object_get_number(rotation_json, "y");
                    transform_component->rotation.z = json_object_get_number(rotation_json, "z");
                }
                if (scale_json) {
                    transform_component->scale.x = json_object_get_number(scale_json, "x");
                    transform_component->scale.y = json_object_get_number(scale_json, "y");
                    transform_component->scale.z = json_object_get_number(scale_json, "z");
                }
                // Build the model matrix
                Matrix4 translation_matrix = mat4_create_translation_matrix(transform_component->position.x, transform_component->position.y, transform_component->position.z);
                Matrix4 rotation_x_matrix = mat4_create_rotation_x_matrix(transform_component->rotation.x);
                Matrix4 rotation_y_matrix = mat4_create_rotation_y_matrix(transform_component->rotation.y);
                Matrix4 rotation_z_matrix = mat4_create_rotation_z_matrix(transform_component->rotation.z);
                Matrix4 rotation_matrix = mat4_multiply(rotation_z_matrix, mat4_multiply(rotation_y_matrix, rotation_x_matrix));
                Matrix4 scale_matrix = mat4_create_scaling_matrix(transform_component->scale.x, transform_component->scale.y, transform_component->scale.z);
                transform_component->model_matrix = mat4_multiply(translation_matrix, mat4_multiply(rotation_matrix, scale_matrix));
            }
        }
        JSON_Object *mesh_json = json_object_get_object(components, "mesh");
        if (mesh_json) {
            JSON_Array *vertices_array = json_object_get_array(mesh_json, "vertices");
            JSON_Array *indices_array = json_object_get_array(mesh_json, "indices");
            if (vertices_array && indices_array) {
                int vertex_count = json_array_get_count(vertices_array);
                int triangle_count = json_array_get_count(indices_array) / 3;

                scene->component_masks[entity_id] |= (1ULL << MESH); // Mark that this entity has a mesh component
                MeshComponent *mesh_component = get_component(scene, MESH, entity_id);
                if (mesh_component) {
                    mesh_component->triangle_count = triangle_count;
                    mesh_component->triangles = malloc(triangle_count * sizeof(Triangle3D));
                    for (int j = 0; j < triangle_count; j++) {
                        int vertex_index_0 = (int)json_array_get_number(indices_array, j * 3);
                        if (vertex_index_0 < 0 || vertex_index_0 >= vertex_count) {
                            continue; // Skip invalid vertex index
                        }
                        int vertex_index_1 = (int)json_array_get_number(indices_array, j * 3 + 1);
                        if (vertex_index_1 < 0 || vertex_index_1 >= vertex_count) {
                            continue; // Skip invalid vertex index
                        }
                        int vertex_index_2 = (int)json_array_get_number(indices_array, j * 3 + 2);
                        if (vertex_index_2 < 0 || vertex_index_2 >= vertex_count) {
                            continue; // Skip invalid vertex index
                        }
                        JSON_Object *vertex_json_0 = json_array_get_object(vertices_array, vertex_index_0);
                        JSON_Object *vertex_json_1 = json_array_get_object(vertices_array, vertex_index_1);
                        JSON_Object *vertex_json_2 = json_array_get_object(vertices_array, vertex_index_2);
                        if (vertex_json_0 && vertex_json_1 && vertex_json_2) {
                            mesh_component->triangles[j].vertices[0].position.x = json_object_get_number(vertex_json_0, "x");
                            mesh_component->triangles[j].vertices[0].position.y = json_object_get_number(vertex_json_0, "y");
                            mesh_component->triangles[j].vertices[0].position.z = json_object_get_number(vertex_json_0, "z");
                            mesh_component->triangles[j].vertices[1].position.x = json_object_get_number(vertex_json_1, "x");
                            mesh_component->triangles[j].vertices[1].position.y = json_object_get_number(vertex_json_1, "y");
                            mesh_component->triangles[j].vertices[1].position.z = json_object_get_number(vertex_json_1, "z");
                            mesh_component->triangles[j].vertices[2].position.x = json_object_get_number(vertex_json_2, "x");
                            mesh_component->triangles[j].vertices[2].position.y = json_object_get_number(vertex_json_2, "y");
                            mesh_component->triangles[j].vertices[2].position.z = json_object_get_number(vertex_json_2, "z");
                            mesh_component->triangles[j].vertices[0].color = 0xFFFFFFFF; // Default color (white)
                            mesh_component->triangles[j].vertices[1].color = 0xFFFFFFFF; // Default color (white)
                            mesh_component->triangles[j].vertices[2].color = 0xFFFFFFFF; // Default color (white)
                            mesh_component->triangles[j].color = 0xFFFFFFFF; // Default color (white)
                        }
                    }
                }
            }
        }
    }
    json_value_free(root_value);
    return scene;
}
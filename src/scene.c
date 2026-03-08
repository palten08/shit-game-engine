#include "../include/scene.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/parson.h"
#include "../include/ecs.h"

Scene initialize_scene() {
    Scene scene = {0};
    scene.entity_manager.next_id = 0; // Start entity IDs from 0
    // Register built-in engine ECS components
    TRANSFORM = register_component(&scene, sizeof(TransformComponent), "transform", parse_transform_component);
    MESH = register_component(&scene, sizeof(MeshComponent), "mesh", parse_mesh_component);
    return scene;
}

Scene *load_scene_from_file(Scene *scene, const char *filename) {
    JSON_Value *root_value = json_parse_file(filename);
    if (!root_value) {
        return scene; // Return empty scene if file cannot be parsed
    }
    JSON_Object *root_object = json_value_get_object(root_value);

    JSON_Object *asset_library_json = json_object_get_object(root_object, "asset_library");
    if (!asset_library_json) {
        json_value_free(root_value);
        return scene; // Don't support not having an asset library
    }

    JSON_Array *meshes_array = json_object_get_array(asset_library_json, "meshes");
    if (meshes_array) {
        scene->asset_library.mesh_count = json_array_get_count(meshes_array);
        scene->asset_library.meshes = malloc(sizeof(Mesh3D) * scene->asset_library.mesh_count);
        for (uint32_t i = 0; i < scene->asset_library.mesh_count; i++) {
            JSON_Object *mesh_json = json_array_get_object(meshes_array, i);
            if (mesh_json) {
                JSON_Array *vertices_array = json_object_get_array(mesh_json, "vertices");
                JSON_Array *indices_array = json_object_get_array(mesh_json, "indices");
                if (!vertices_array || !indices_array) {
                    return scene; // Invalid mesh data
                }
                Mesh3D loaded_mesh = {0};    
                int vertex_count = json_array_get_count(vertices_array);
                int triangle_count = json_array_get_count(indices_array) / 3;

                loaded_mesh.triangle_count = triangle_count;
                loaded_mesh.triangles = malloc(triangle_count * sizeof(Triangle3D));
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
                        loaded_mesh.triangles[j].vertices[0].position.x = json_object_get_number(vertex_json_0, "x");
                        loaded_mesh.triangles[j].vertices[0].position.y = json_object_get_number(vertex_json_0, "y");
                        loaded_mesh.triangles[j].vertices[0].position.z = json_object_get_number(vertex_json_0, "z");
                        loaded_mesh.triangles[j].vertices[1].position.x = json_object_get_number(vertex_json_1, "x");
                        loaded_mesh.triangles[j].vertices[1].position.y = json_object_get_number(vertex_json_1, "y");
                        loaded_mesh.triangles[j].vertices[1].position.z = json_object_get_number(vertex_json_1, "z");
                        loaded_mesh.triangles[j].vertices[2].position.x = json_object_get_number(vertex_json_2, "x");
                        loaded_mesh.triangles[j].vertices[2].position.y = json_object_get_number(vertex_json_2, "y");
                        loaded_mesh.triangles[j].vertices[2].position.z = json_object_get_number(vertex_json_2, "z");
                        // Random color
                        uint32_t random_color = (rand() % 0xFFFFFF) | 0xFF000000; // Random color with full alpha
                        loaded_mesh.triangles[j].vertices[0].color = random_color;
                        loaded_mesh.triangles[j].vertices[1].color = random_color;
                        loaded_mesh.triangles[j].vertices[2].color = random_color;
                        loaded_mesh.triangles[j].color = random_color;
                    }
                }
                scene->asset_library.meshes[i] = loaded_mesh;
            }
        }
    }

    // Virtual camera loading
    JSON_Object *camera_json = json_object_get_object(root_object, "camera");
    if (!camera_json) {
        json_value_free(root_value);
        return scene; // Don't support not having a camera
    }
    JSON_Object *position_json = json_object_get_object(camera_json, "position");
    JSON_Object *rotation_json = json_object_get_object(camera_json, "rotation");
    JSON_Object *look_target_json = json_object_get_object(camera_json, "look_target");
    if (!position_json || !rotation_json) {
        json_value_free(root_value);
        return scene; // Don't support not having a camera position or rotation
    }
    scene->virtual_camera = initialize_virtual_camera(json_object_get_number(camera_json, "aspect_ratio"), json_object_get_number(camera_json, "field_of_view"), json_object_get_number(camera_json, "near_plane"), json_object_get_number(camera_json, "far_plane"), (Vector3f){json_object_get_number(position_json, "x"), json_object_get_number(position_json, "y"), json_object_get_number(position_json, "z")}, (Vector3f){json_object_get_number(rotation_json, "x"), json_object_get_number(rotation_json, "y"), json_object_get_number(rotation_json, "z")}, (Vector3f){json_object_get_number(look_target_json, "x"), json_object_get_number(look_target_json, "y"), json_object_get_number(look_target_json, "z")});

    // Entity loading
    JSON_Array *entities_array = json_object_get_array(root_object, "entities");
    for (int i = 0; i < 1024; i++) {
        JSON_Object *entity_json = json_array_get_object(entities_array, i);
        if (!entity_json) {
            break; // No more entities in the array
        }
        Entity entity_id = register_entity(scene);
        JSON_Object *components = json_object_get_object(entity_json, "components");
        for (int j = 0; j < 24; j++) {
            JSON_Object *component_json = json_object_get_object(components, json_object_get_name(components, j));
            if (!component_json) {
                break; // No more components for this entity
            }
            int component_id = -1;
            for (int k = 0; k < scene->registered_component_count; k++) {
                if (strcmp(scene->component_array[k].name, json_object_get_name(components, j)) == 0) {
                    component_id = k;
                    break;
                }
            }
            if (component_id == -1) {
                continue; // Component not registered, skip it
            }
            scene->component_masks[entity_id] |= (1ULL << component_id); // Set the bit for this component in the entity's component mask
            scene->component_array[component_id].parser(scene, entity_id, component_id, component_json);
        }
    }
    json_value_free(root_value);
    return scene;
}
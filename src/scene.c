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

    // Virtual camera loading
    JSON_Object *camera_json = json_object_get_object(root_object, "camera");
    if (!camera_json) {
        json_value_free(root_value);
        return scene; // Don't support not having a camera
    }
    JSON_Object *position_json = json_object_get_object(camera_json, "position");
    JSON_Object *rotation_json = json_object_get_object(camera_json, "rotation");
    if (!position_json || !rotation_json) {
        json_value_free(root_value);
        return scene; // Don't support not having a camera position or rotation
    }
    scene->virtual_camera = initialize_virtual_camera(json_object_get_number(camera_json, "aspect_ratio"), json_object_get_number(camera_json, "field_of_view"), json_object_get_number(camera_json, "near_plane"), json_object_get_number(camera_json, "far_plane"), (Vector3f){json_object_get_number(position_json, "x"), json_object_get_number(position_json, "y"), json_object_get_number(position_json, "z")}, (Vector3f){json_object_get_number(rotation_json, "x"), json_object_get_number(rotation_json, "y"), json_object_get_number(rotation_json, "z")});

    // Entity loading
    JSON_Array *entities_array = json_object_get_array(root_object, "entities");
    for (int i = 0; i < 256; i++) {
        JSON_Object *entity_json = json_array_get_object(entities_array, i);
        if (!entity_json) {
            break; // No more entities in the array
        }
        Entity entity_id = register_entity(scene);
        JSON_Object *components = json_object_get_object(entity_json, "components");
        for (int j = 0; j < 256; j++) {
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
#include "../include/scene.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/vector_operations.h"
#include "../include/parson.h"
#include "../include/ecs.h"
#include "../include/obj_loader.h"
#include "../include/logging.h"

Scene *load_scene_from_file(Scene *scene, const char *filename) {
    LOG_DEBUG("Begin scene loading function");
    JSON_Value *root_value = json_parse_file(filename);
    if (!root_value) {
        LOG_WARNING("Given scene file could not be parsed");
        return scene; // Return empty scene if file cannot be parsed
    }
    LOG_DEBUG("Getting root object JSON");
    JSON_Object *root_object = json_value_get_object(root_value);
    if (!root_object) {
        LOG_WARNING("Root object of the given scene file could not be parsed");
        return scene;
    }

    LOG_DEBUG("Getting asset library JSON");
    JSON_Object *asset_library_json = json_object_get_object(root_object, "asset_library");
    if (!asset_library_json) {
        LOG_WARNING("Could not parse the asset library from the given scene file");
        json_value_free(root_value);
        return scene; // Don't support not having an asset library
    }

    LOG_DEBUG("Getting directional light JSON");
    JSON_Object *directional_light_json = json_object_get_object(root_object, "directional_light");
    if (directional_light_json) {
        JSON_Object *direction_json = json_object_get_object(directional_light_json, "direction");
        if (!direction_json) {
            json_value_free(root_value);
            return scene; // Don't support not having a directional light direction
        }
        scene->directional_light.direction = vec3f_normalize((Vector3f){json_object_get_number(direction_json, "x"), json_object_get_number(direction_json, "y"), json_object_get_number(direction_json, "z")});
        JSON_Object *color_json = json_object_get_object(directional_light_json, "color");
        if (color_json) {
            uint8_t r = (uint8_t)(json_object_get_number(color_json, "r") * 255.0f);
            uint8_t g = (uint8_t)(json_object_get_number(color_json, "g") * 255.0f);
            uint8_t b = (uint8_t)(json_object_get_number(color_json, "b") * 255.0f);
            uint32_t packed_color = 0xFF000000 | (r << 16) | (g << 8) | b;
            scene->directional_light.color = packed_color;
        }
        scene->directional_light.intensity = json_object_get_number(directional_light_json, "intensity");
        scene->directional_light.ambient_intensity = json_object_get_number(directional_light_json, "ambient_intensity");
    }

    LOG_DEBUG("Getting mesh array JSON");
    JSON_Array *meshes_array = json_object_get_array(asset_library_json, "meshes");
    if (meshes_array) {
        scene->asset_library.mesh_count = json_array_get_count(meshes_array);
        scene->asset_library.meshes = malloc(sizeof(Mesh3D) * scene->asset_library.mesh_count);
        for (uint32_t i = 0; i < scene->asset_library.mesh_count; i++) {
            JSON_Object *mesh_json = json_array_get_object(meshes_array, i);
            if (mesh_json) {
                const char *file_path = json_object_get_string(mesh_json, "file_path");
                if (file_path) {
                    Mesh3D loaded_mesh = load_obj(file_path);
                    if (loaded_mesh.triangle_count > 0) {
                        scene->asset_library.meshes[i] = loaded_mesh;
                    }
                }
            }
        }
    }

    LOG_DEBUG("Getting virtual camera JSON");
    JSON_Object *camera_json = json_object_get_object(root_object, "camera");
    if (!camera_json) {
        json_value_free(root_value);
        return scene; // Don't support not having a camera
    }
    LOG_DEBUG("Initializing virtual camera");
    JSON_Object *position_json = json_object_get_object(camera_json, "position");
    JSON_Object *rotation_json = json_object_get_object(camera_json, "rotation");
    JSON_Object *look_target_json = json_object_get_object(camera_json, "look_target");
    if (!position_json || !rotation_json) {
        LOG_ERROR("Specified virtual camera has no position or rotation JSON");
        json_value_free(root_value);
        return scene; // Don't support not having a camera position or rotation
    }
    scene->virtual_camera = initialize_virtual_camera(json_object_get_number(camera_json, "aspect_ratio"), json_object_get_number(camera_json, "field_of_view"), json_object_get_number(camera_json, "near_plane"), json_object_get_number(camera_json, "far_plane"), (Vector3f){json_object_get_number(position_json, "x"), json_object_get_number(position_json, "y"), json_object_get_number(position_json, "z")}, (Vector3f){json_object_get_number(rotation_json, "x"), json_object_get_number(rotation_json, "y"), json_object_get_number(rotation_json, "z")}, (Vector3f){json_object_get_number(look_target_json, "x"), json_object_get_number(look_target_json, "y"), json_object_get_number(look_target_json, "z")});

    LOG_DEBUG("Getting entities array JSON");
    JSON_Array *entities_array = json_object_get_array(root_object, "entities");
    for (int i = 0; i < 1024; i++) {
        JSON_Object *entity_json = json_array_get_object(entities_array, i);
        if (!entity_json) {
            break; // No more entities in the array
        }
        LOG_DEBUG("Processing entity with ID %d", i);
        Entity entity_id = register_entity(scene);
        LOG_DEBUG("Getting components JSON for entity %d", i);
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
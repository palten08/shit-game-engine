#include "../include/ecs.h"
#include "../include/types.h"
#include "../include/scene.h"
#include "../include/matrix_operations.h"

int TRANSFORM;
int MESH;

int register_entity(Scene *scene) {
    if (scene->registered_entity_count >= MAX_ENTITIES) {
        return -1; // No available slot for the entity
    }
    int entity_id = scene->entity_manager.next_id++;
    scene->registered_entity_count++;
    return entity_id;
}

int register_component(Scene *scene, size_t component_size, const char *name, ComponentParserFunction parser) {
    if (scene->registered_component_count >= MAX_COMPONENTS) {
        return -1; // No available slot for the component
    }
    int component_id = scene->registered_component_count;
    scene->component_array[component_id].size = component_size;
    strncpy(scene->component_array[component_id].name, name, sizeof(scene->component_array[component_id].name) - 1);
    scene->component_array[component_id].name[sizeof(scene->component_array[component_id].name) - 1] = '\0';
    scene->component_array[component_id].parser = parser;
    scene->component_array[component_id].data = malloc(MAX_ENTITIES * component_size);
    scene->component_array[component_id].count = 0;
    scene->registered_component_count++;
    return component_id;
}

void *get_component(Scene *scene, int component_id, Entity entity) {
    if (component_id < 0 || component_id >= scene->registered_component_count) {
        return NULL; // Invalid component ID
    }
    if (entity < 0 || entity >= MAX_ENTITIES) {
        return NULL; // Invalid entity ID
    }
    if ((scene->component_masks[entity] & (1ULL << component_id)) == 0) {
        return NULL; // Entity does not have this component
    }
    return (void *)(char *)(scene->component_array[component_id].data + (entity * scene->component_array[component_id].size));
}

int register_system(Scene *scene, void (*system_function)(Scene *, double), uint64_t required_components) {
    if (scene->registered_system_count >= MAX_SYSTEMS) {
        return -1; // No available slot for the system
    }
    int system_id = scene->registered_system_count;
    scene->systems[system_id].function = system_function;
    scene->systems[system_id].required_components = required_components;
    scene->registered_system_count++;
    return system_id;
}

void *get_system(Scene *scene, int system_id) {
    if (system_id < 0 || system_id >= scene->registered_system_count) {
        return NULL; // Invalid system ID
    }
    return (void *)&scene->systems[system_id];
}

void run_systems(Scene *scene, double delta_time) {
    for (int i = 0; i < scene->registered_system_count; i++) {
        System *system = &scene->systems[i];
        system->function(scene, delta_time);
    }
}

void parse_transform_component(Scene *scene, Entity entity, int component_id, JSON_Object *json) {
    TransformComponent *transform_component = get_component(scene, component_id, entity);
    if (!transform_component) {
        return; // Failed to get the component
    }
    JSON_Object *position_json = json_object_get_object(json, "position");
    JSON_Object *rotation_json = json_object_get_object(json, "rotation");
    JSON_Object *scale_json = json_object_get_object(json, "scale");
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
    Matrix4 translation_matrix = mat4_create_translation_matrix(transform_component->position.x, transform_component->position.y, transform_component->position.z);
    Matrix4 rotation_x_matrix = mat4_create_rotation_x_matrix(transform_component->rotation.x);
    Matrix4 rotation_y_matrix = mat4_create_rotation_y_matrix(transform_component->rotation.y);
    Matrix4 rotation_z_matrix = mat4_create_rotation_z_matrix(transform_component->rotation.z);
    Matrix4 rotation_matrix = mat4_multiply(rotation_z_matrix, mat4_multiply(rotation_y_matrix, rotation_x_matrix));
    Matrix4 scale_matrix = mat4_create_scaling_matrix(transform_component->scale.x, transform_component->scale.y, transform_component->scale.z);
    transform_component->model_matrix = mat4_multiply(translation_matrix, mat4_multiply(rotation_matrix, scale_matrix));
}

void parse_mesh_component(Scene *scene, Entity entity, int component_id, JSON_Object *json) {
    printf("Parsing mesh component for entity %d\n", entity);
    MeshComponent *mesh_component = get_component(scene, component_id, entity);
    if (!mesh_component) {
        return; // Failed to get the component
    }
    JSON_Array *vertices_array = json_object_get_array(json, "vertices");
    JSON_Array *indices_array = json_object_get_array(json, "indices");
    if (!vertices_array || !indices_array) {
        return; // Invalid mesh data
    }
    
    int vertex_count = json_array_get_count(vertices_array);
    int triangle_count = json_array_get_count(indices_array) / 3;

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
            // Random color
            uint32_t random_color = (rand() % 0xFFFFFF) | 0xFF000000; // Random color with full alpha
            mesh_component->triangles[j].vertices[0].color = random_color;
            mesh_component->triangles[j].vertices[1].color = random_color;
            mesh_component->triangles[j].vertices[2].color = random_color;
            mesh_component->triangles[j].color = random_color;
        }
    }
}
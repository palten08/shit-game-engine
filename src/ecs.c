#include "../include/ecs.h"
#include "../include/types.h"
#include "../include/scene.h"

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

int register_component(Scene *scene, size_t component_size) {
    if (scene->registered_component_count >= MAX_COMPONENTS) {
        return -1; // No available slot for the component
    }
    int component_id = scene->registered_component_count;
    scene->component_array[component_id].size = component_size;
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
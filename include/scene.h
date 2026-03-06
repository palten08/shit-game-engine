#pragma once

#include "types.h"

#include "virtual_camera.h"

#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 32
#define MAX_COMPONENTS 24

/**
 * @brief A structure representing a scene, including its entities and components.
 */
typedef struct Scene {
    uint64_t component_masks[MAX_ENTITIES]; // 8192 bytes
    ComponentArray component_array[MAX_COMPONENTS]; // 1440 bytes
    System systems[MAX_SYSTEMS]; // 512 bytes
    VirtualCamera virtual_camera; // 80 bytes
    int registered_entity_count; // 4 bytes
    int registered_component_count; // 4 bytes
    int registered_system_count; // 4 bytes
    EntityManager entity_manager; // 4 bytes
} Scene;

Scene initialize_scene();
Scene *load_scene_from_file(Scene *scene, const char *filename);
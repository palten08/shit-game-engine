#pragma once

#include "types.h"
#include "ecs_types.h"

#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 32
#define MAX_COMPONENTS 24

/**
 * @brief A structure representing a scene, including its entities and components.
 */
typedef struct {
    uint64_t component_masks[MAX_ENTITIES]; // 8192 bytes
    ComponentArray component_array[MAX_COMPONENTS];
    System systems[MAX_SYSTEMS];
    int entity_count; // 4 bytes
    int registered_component_count; // 4 bytes
    EntityManager entity_manager; // 4 bytes
} Scene;

Scene create_scene_from_file(const char *filename);
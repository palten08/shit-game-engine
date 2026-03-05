#pragma once

#include "types.h"
#include "scene.h"


int register_entity(Scene *scene);
int register_component(Scene *scene, size_t component_size);
void *get_component(Scene *scene, int component_id, Entity entity);
int register_system(Scene *scene, void (*system_function)(Scene *, double), uint64_t required_components);
void *get_system(Scene *scene, int system_id);
void run_systems(Scene *scene, double delta_time);
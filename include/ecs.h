#pragma once

#include "types.h"
#include "scene.h"
#include "app.h"

int register_entity(Scene *scene);
int register_component(Scene *scene, size_t component_size, const char *name, ComponentParserFunction parser);
void *get_component(Scene *scene, int component_id, Entity entity);
int register_system(Scene *scene, void (*system_function)(Scene *, AppContext *), uint64_t required_components);
void *get_system(Scene *scene, int system_id);
void run_systems(Scene *scene, AppContext *app_context);

void parse_transform_component(Scene *scene, Entity entity, int component_id, JSON_Object *json);
void parse_mesh_component(Scene *scene, Entity entity, int component_id, JSON_Object *json);
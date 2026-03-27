#pragma once

#include "types.h"
#include "scene.h"
#include "app.h"

int register_entity(Scene *scene);
bool register_entity_record(Scene *scene, int entity_id, int archetype_index, int archetype_table_row_index);
EntityRecord get_entity_record(Scene *scene, Entity entity);
int register_component(Scene *scene, size_t component_size, const char *name, ComponentParserFunction parser);
void *get_component(Scene *scene, int component_id, Entity entity);
int register_system(Scene *scene, void (*system_function)(Scene *, AppContext *), uint64_t required_components);
void *get_system(Scene *scene, int system_id);
void run_systems(Scene *scene, AppContext *app_context);

Archetype create_new_archetype(Scene *scene, uint64_t component_mask);
int add_row_to_archetype(Archetype *archetype, Entity entity_id);
int remove_row_from_archetype(Scene *scene, Archetype *archetype, Entity entity_id);
void *read_component_data_from_archetype_by_component_id(Archetype *archetype, int component_id, int row_index);
void *read_component_data_from_archetype(Archetype *archetype, int column_index, int row_index);

void parse_transform_component(Scene *scene, Entity entity, int component_id, void *data);
void parse_mesh_component(Scene *scene, Entity entity, int component_id, void *data);
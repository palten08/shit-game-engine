#include <stdlib.h>

#include "../include/ecs.h"
#include "../include/types.h"
#include "../include/scene.h"
#include "../include/app.h"
#include "../include/matrix_operations.h"
#include "../include/quaternion_operations.h"

int TRANSFORM;
int MESH;

int register_entity(Scene *scene) {
    if (scene->entity_manager.next_id >= MAX_ENTITIES) {
        LOG_WARNING("No room to register new entity; Registered entity count is %d", scene->registered_entity_count);
        return -1; // No available slot for the entity
    }
    int entity_id = scene->entity_manager.next_id;
    scene->entity_manager.next_id++;
    EntityRecord empty_record = { .archetype_index = -1, .archetype_row_index = -1 };
    scene->entity_records[entity_id] = empty_record;
    scene->registered_entity_count++;
    return entity_id;
}

int get_entity_id_by_name(Scene *scene, const char *name) {
    for (int i = 0; i < scene->registered_entity_count; i++) {
        if (strcmp(scene->entity_name_to_id_map[i].entity_name, name) == 0) {
            return scene->entity_name_to_id_map[i].entity_id;
        }
    }
    LOG_WARNING("Could not find entity with name '%s'", name);
    return -1; // Entity with the given name was not found
}

bool register_entity_record(Scene *scene, int entity_id, int archetype_index, int archetype_row_index) {
    if (entity_id < 0 || entity_id >= MAX_ENTITIES) {
        LOG_WARNING("Invalid entity ID %d", entity_id);
        return false; // Invalid entity ID
    }
    scene->entity_records[entity_id].archetype_index = archetype_index;
    scene->entity_records[entity_id].archetype_row_index = archetype_row_index;
    return true;
}

EntityRecord get_entity_record(Scene *scene, Entity entity) {
    if (entity < 0 || entity >= MAX_ENTITIES) {
        LOG_WARNING("Invalid entity ID %d", entity);
        return (EntityRecord){ .archetype_index = -1, .archetype_row_index = -1 }; // Invalid entity ID
    }
    return scene->entity_records[entity];
}

int register_component(Scene *scene, size_t component_size, const char *name, ComponentParserFunction parser) {
    if (scene->registered_component_count >= MAX_COMPONENTS) {
        LOG_WARNING("No room to register new component ('%s'); Registered component count is %d", name, scene->registered_component_count);
        return -1; // No available slot for the component
    }
    int component_id = scene->registered_component_count;
    LOG_DEBUG("Registering component with ID %d", component_id);
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
    EntityRecord current_entity_record = get_entity_record(scene, entity);
    if (current_entity_record.archetype_index < 0 || current_entity_record.archetype_row_index < 0) {
        LOG_WARNING("Entity %d does not have a valid archetype record, cannot get component with ID %d", entity, component_id);
        return NULL; // Entity does not have a valid archetype record
    }
    Archetype *current_archetype = &scene->archetypes[current_entity_record.archetype_index];
    if ((current_archetype->component_mask & (1ULL << component_id)) == 0) {
        LOG_DEBUG("Entity %d does not have component with ID %d", entity, component_id);
        return NULL; // Entity does not have this component
    }
    return read_component_data_from_archetype_by_component_id(current_archetype, component_id, current_entity_record.archetype_row_index);
}

int register_system(Scene *scene, void (*system_function)(Scene *, AppContext *), uint64_t required_components) {
    if (scene->registered_system_count >= MAX_SYSTEMS) {
        LOG_WARNING("No room to register new system; Registered system count is %d", scene->registered_system_count);
        return -1; // No available slot for the system
    }
    int system_id = scene->registered_system_count;
    LOG_DEBUG("Registering system with ID %d", system_id);
    scene->systems[system_id].function = system_function;
    scene->systems[system_id].required_components = required_components;
    scene->registered_system_count++;
    return system_id;
}

void *get_system(Scene *scene, int system_id) {
    if (system_id < 0 || system_id >= scene->registered_system_count) {
        LOG_WARNING("Could not find system with ID %d", system_id);
        return NULL; // Invalid system ID
    }
    return (void *)&scene->systems[system_id];
}

void run_systems(Scene *scene, AppContext *app_context) {
    for (int i = 0; i < scene->registered_system_count; i++) {
        System *system = &scene->systems[i];
        system->function(scene, app_context);
    }
}

Archetype *create_new_archetype(Scene *scene, uint64_t component_mask) {
    Archetype *archetype = &scene->archetypes[scene->registered_archetype_count++];
    archetype->component_mask = component_mask;
    archetype->entity_ids = malloc(sizeof(Entity) * MAX_ENTITIES);
    archetype->row_count = 0;
    archetype->row_capacity = MAX_ENTITIES;

    // Count how many columns this archetype table will have (number of bits set in the mask)
    int column_count = 0;
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (component_mask & (1ULL << i)) column_count++;
    }

    archetype->columns = malloc(sizeof(ArchetypeColumn) * column_count);
    archetype->component_to_column_map = malloc(sizeof(ComponentToColumnMapEntry) * column_count);
    archetype->column_count = column_count;

    int current_column = 0;
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (component_mask & (1ULL << i)) {
            archetype->component_to_column_map[current_column].component_id = i;
            archetype->component_to_column_map[current_column].column_index = current_column;
            archetype->columns[current_column].component_id = i;
            archetype->columns[current_column].component_size = scene->component_array[i].size;
            archetype->columns[current_column].component_structures = malloc(scene->component_array[i].size * MAX_ENTITIES);
            current_column++;
        }
    }

    return archetype;
}

Archetype *find_or_create_archetype(Scene *scene, uint64_t component_mask) {
    for (int i = 0; i < scene->registered_archetype_count; i++) {
        if (scene->archetypes[i].component_mask == component_mask) {
            return &scene->archetypes[i];
        }
    }
    return create_new_archetype(scene, component_mask);
}

int add_row_to_archetype(Archetype *archetype, Entity entity_id) {
    int row_index = archetype->row_count;
    if (archetype->row_count >= archetype->row_capacity) {
        LOG_DEBUG("Archetype table is full, doubling capacity");
        archetype->row_capacity *= 2;
        archetype->entity_ids = realloc(archetype->entity_ids, sizeof(Entity) * archetype->row_capacity);
        for (int i = 0; i < archetype->column_count; i++) {
            archetype->columns[i].component_structures = realloc(archetype->columns[i].component_structures, archetype->columns[i].component_size * archetype->row_capacity);
        }
    }
    archetype->entity_ids[archetype->row_count] = entity_id;
    archetype->row_count++;
    return row_index;
}

int remove_row_from_archetype(Scene *scene, Archetype *archetype, Entity entity_id) {
    EntityRecord current_entity_record = get_entity_record(scene, entity_id);
    if (current_entity_record.archetype_index < 0 || current_entity_record.archetype_row_index < 0) {
        LOG_WARNING("Entity %d does not have a valid archetype record, cannot remove from archetype", entity_id);
        return -1; // Entity does not have a valid archetype record
    }
    int current_row_index = current_entity_record.archetype_row_index;
    if (current_row_index < 0 || current_row_index >= archetype->row_count) {
        LOG_WARNING("Invalid row index %d for entity %d in archetype table", current_row_index, entity_id);
        return -1; // Invalid row index
    }
    int last_row_index = archetype->row_count - 1;
    if (last_row_index < 0) {
        LOG_WARNING("Archetype table is empty, cannot remove row for entity %d", entity_id);
        return -1; // Archetype table is empty
    }
    if (current_row_index != last_row_index) {
        // The entity we are trying to remove is not the last one in the table
        EntityRecord last_entity_record = get_entity_record(scene, archetype->entity_ids[archetype->row_count - 1]);
        if (last_entity_record.archetype_index < 0 || last_entity_record.archetype_row_index < 0) {
            LOG_WARNING("Last entity in archetype table does not have a valid archetype record", archetype->entity_ids[archetype->row_count - 1]);
            return -1; // Last entity in archetype table does not have a valid archetype record
        }
        Entity last_entity_id = archetype->entity_ids[last_row_index];
        archetype->entity_ids[current_row_index] = archetype->entity_ids[last_row_index];
        for (int i = 0; i < archetype->column_count; i++) {
            size_t component_size = archetype->columns[i].component_size;
            void *current_component_data = (char *)archetype->columns[i].component_structures + (current_row_index * component_size);
            void *last_component_data = (char *)archetype->columns[i].component_structures + (last_row_index * component_size);
            memcpy(current_component_data, last_component_data, component_size);
        }
        //register_entity_record(scene, entity_id, -1, -1); // Invalidate the entity's archetype record
        register_entity_record(scene, archetype->entity_ids[current_row_index], last_entity_record.archetype_index, current_row_index); // Update the moved entity's archetype record
        archetype->row_count--;
        return last_entity_id;
    } else {
        // The entity we are trying to remove is the last one in the table, we can just decrease the row count
        archetype->row_count--;
        //register_entity_record(scene, entity_id, -1, -1); // Invalidate the entity's archetype record
        return -1;
    }
}

void remove_component_from_entity(Scene *scene, Entity entity, int component_id) {
    EntityRecord current_entity_record = get_entity_record(scene, entity);
    if (current_entity_record.archetype_index < 0 || current_entity_record.archetype_row_index < 0) {
        LOG_WARNING("Entity %d does not have a valid archetype record, cannot remove component", entity);
        return; // Entity does not have a valid archetype record
    }
    if ((scene->archetypes[current_entity_record.archetype_index].component_mask & (1ULL << component_id)) == 0) {
        LOG_WARNING("Entity %d does not have component with ID %d, cannot remove", entity, component_id);
        return; // Entity does not have this component
    }
    uint64_t resultant_component_mask = scene->archetypes[current_entity_record.archetype_index].component_mask & ~(1ULL << component_id);
    if (resultant_component_mask == 0) {
        // The entity will have no components left after this, so we can just remove it from its current archetype and not add it to a new archetype
        remove_row_from_archetype(scene, &scene->archetypes[current_entity_record.archetype_index], entity);
        register_entity_record(scene, entity, -1, -1); // Invalidate the entity's archetype record
    } else {
        // The entity will still have at least one component
        // We need to find a new archetype for it to exist under
        Archetype *resultant_archetype = find_or_create_archetype(scene, resultant_component_mask);
        int new_row_index = add_row_to_archetype(resultant_archetype, entity);
        // Copy existing component data (except for the removed component) to the new archetype's structure
        Archetype *current_archetype = &scene->archetypes[current_entity_record.archetype_index];
        for (int i = 0; i < current_archetype->column_count; i++) {
            int existing_component_id = current_archetype->component_to_column_map[i].component_id;
            if (existing_component_id == component_id) continue; // Skip the removed component
            void *source = read_component_data_from_archetype(current_archetype, i, current_entity_record.archetype_row_index);
            void *destination = read_component_data_from_archetype_by_component_id(resultant_archetype, existing_component_id, new_row_index);
            memcpy(destination, source, scene->component_array[existing_component_id].size);
        }
        // Remove the entity's row from the old archetype
        remove_row_from_archetype(scene, current_archetype, entity);

        register_entity_record(scene, entity, resultant_archetype - scene->archetypes, new_row_index);
    }
}

void destroy_entity(Scene *scene, Entity entity) {
    EntityRecord current_entity_record = get_entity_record(scene, entity);
    if (current_entity_record.archetype_index < 0 || current_entity_record.archetype_row_index < 0) {
        // Entity already does not have a valid archetype record, so we can just return
        return;
    }
    Archetype *current_archetype = &scene->archetypes[current_entity_record.archetype_index];
    remove_row_from_archetype(scene, current_archetype, entity);
    register_entity_record(scene, entity, -1, -1); // Invalidate the entity's archetype record
}

void *read_component_data_from_archetype_by_component_id(Archetype *archetype, int component_id, int row_index) {
    for (int i = 0; i < archetype->column_count; i++) {
        if (archetype->columns[i].component_id == component_id) {
            return (char *)archetype->columns[i].component_structures + (row_index * archetype->columns[i].component_size);
        }
    }
    LOG_WARNING("Component with ID %d not found in archetype table", component_id);
    return NULL; // Component not found in archetype table
}

void *read_component_data_from_archetype(Archetype *archetype, int column_index, int row_index) {
    return (char *)archetype->columns[column_index].component_structures + (row_index * archetype->columns[column_index].component_size);
}

void add_component(Scene *scene, Entity entity, int component_id, void *component_data) {
    EntityRecord current_entity_record = get_entity_record(scene, entity);
    if (current_entity_record.archetype_index < 0) {
        // The entity is not associated with any archetypes at all
        // Either we find one that already exists and has only this component or we create a new archetype for it
        uint64_t new_component_mask = (1ULL << component_id);
        Archetype *resultant_archetype = find_or_create_archetype(scene, new_component_mask);
        int new_row_index = add_row_to_archetype(resultant_archetype, entity);
        void *destination = read_component_data_from_archetype_by_component_id(resultant_archetype, component_id, new_row_index);
        memcpy(destination, component_data, scene->component_array[component_id].size);
        register_entity_record(scene, entity, resultant_archetype - scene->archetypes, new_row_index);
    } else {
        // The entity has been associated with an archetype already
        // We should check to see if the archetype already has this particular component
        // If it does, we should update the data for it
        // Otherwise, we need to move the entity to a new archetype that has the new component added to its mask and copy all of the existing component data over to the new archetype's structure along with the new component data
        Archetype *current_archetype = &scene->archetypes[current_entity_record.archetype_index];
        if ((current_archetype->component_mask & (1ULL << component_id)) != 0) {
            // The archetype has this component, so we just update the data for it
            void *destination = read_component_data_from_archetype_by_component_id(current_archetype, component_id, current_entity_record.archetype_row_index);
            memcpy(destination, component_data, scene->component_array[component_id].size);
        } else {
            // The archetype does not have this component, so we need to move the entity to a new archetype that has the new component added to its mask and copy all of the existing component data over to the new archetype's structure along with the new component data
            uint64_t new_component_mask = current_archetype->component_mask | (1ULL << component_id);
            Archetype *resultant_archetype = find_or_create_archetype(scene, new_component_mask);
            int new_row_index = add_row_to_archetype(resultant_archetype, entity);
            // Copy existing component data to the new archetype's structure
            for (int i = 0; i < current_archetype->column_count; i++) {
                int existing_component_id = current_archetype->component_to_column_map[i].component_id;
                void *source = read_component_data_from_archetype(current_archetype, i, current_entity_record.archetype_row_index);
                void *destination = read_component_data_from_archetype_by_component_id(resultant_archetype, existing_component_id, new_row_index);
                memcpy(destination, source, scene->component_array[existing_component_id].size);
            }
            // Copy new component data to the new archetype's structure
            void *new_component_destination = read_component_data_from_archetype_by_component_id(resultant_archetype, component_id, new_row_index);
            memcpy(new_component_destination, component_data, scene->component_array[component_id].size);
            // Remove the entity's row from the old archetype
            remove_row_from_archetype(scene, current_archetype, entity);
            // Update the entity record to point to the new archetype and row index
            register_entity_record(scene, entity, resultant_archetype - scene->archetypes, new_row_index);
        }
    }
}

MatchedArchetypes find_matching_archetypes(Scene *scene, uint64_t component_mask_query) {
    MatchedArchetypes result;
    result.archetypes = malloc(sizeof(Archetype *) * scene->registered_archetype_count);
    result.archetype_count = 0;
    for (int i = 0; i < scene->registered_archetype_count; i++) {
        if ((scene->archetypes[i].component_mask & component_mask_query) == component_mask_query) {
            result.archetypes[result.archetype_count++] = &scene->archetypes[i];
        }
    }
    return result;
}

void *get_archetype_column_pointer(Archetype *archetype, int component_id) {
    for (int i = 0; i < archetype->column_count; i++) {
        if (archetype->columns[i].component_id == component_id) {
            return archetype->columns[i].component_structures;
        }
    }
    LOG_WARNING("Component with ID %d not found in archetype table", component_id);
    return NULL; // Component not found in archetype table
}

void parse_transform_component(Scene *scene, Entity entity, int component_id, void *data) {
    LOG_DEBUG("Parsing transform component on entity %d", entity);
    TransformComponent *transform_component = get_component(scene, component_id, entity);
    if (!transform_component) {
        LOG_WARNING("Failed to get the transform component on entity %d", entity);
        return; // Failed to get the component
    }

    float *floats = (float *)data;
    LOG_DEBUG("Transform raw data: pos=(%f, %f, %f) rot=(%f, %f, %f) scale=(%f, %f, %f)",
        floats[0], floats[1], floats[2],
        floats[3], floats[4], floats[5],
        floats[6], floats[7], floats[8]);
    transform_component->position = (Vector3f){ floats[0], floats[1], floats[2] };
    // Blender exports transforms in Euler angles
    float roll = floats[3];
    float pitch = floats[4];
    float yaw = floats[5];
    transform_component->rotation = quaternion_from_euler_angles(roll, pitch, yaw);

    transform_component->scale = (Vector3f){ floats[6], floats[7], floats[8] };

    Matrix4 translation_matrix = mat4_create_translation_matrix(transform_component->position.x, transform_component->position.y, transform_component->position.z);
    Matrix4 rotation_matrix = quaternion_to_matrix4(transform_component->rotation);
    Matrix4 scale_matrix = mat4_create_scaling_matrix(transform_component->scale.x, transform_component->scale.y, transform_component->scale.z);
    transform_component->model_matrix = mat4_multiply(translation_matrix, mat4_multiply(rotation_matrix, scale_matrix));
}

void parse_mesh_component(Scene *scene, Entity entity, int component_id, void *data) {
    LOG_DEBUG("Parsing mesh component on entity %d", entity);
    MeshComponent *mesh_component = get_component(scene, component_id, entity);
    if (!mesh_component) {
        LOG_WARNING("Failed to get the mesh component on entity %d", entity);
        return; // Failed to get the component
    }
    uint32_t name_length;
    memcpy(&name_length, data, sizeof(uint32_t));

    char mesh_name[256] = {0};
    memcpy(mesh_name, (char *)data + sizeof(uint32_t), name_length);
    mesh_component->mesh_id = -1;
    for (uint32_t i = 0; i < scene->asset_library.mesh_count; i++) {
        if (strcmp(scene->asset_library.meshes[i].name, mesh_name) == 0) {
            mesh_component->mesh_id = i;
            return;
        }
    }
    LOG_WARNING("Mesh '%s' not found in asset library", mesh_name);
    return;
}
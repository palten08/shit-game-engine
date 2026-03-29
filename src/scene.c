#include <stdlib.h>
#include <stdio.h>

#include "../include/scene.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/vector_operations.h"
#include "../include/ecs.h"
#include "../include/obj_loader.h"
#include "../include/logging.h"
#include "../include/materials.h"

static char *read_packed_string(FILE *file_pointer) {
    uint32_t string_length = 0;
    if (fread(&string_length, sizeof(uint32_t), 1, file_pointer) != 1) {
        LOG_ERROR("Failed to read string length from binary scene file");
        return NULL; // Return NULL if string length cannot be read
    }
    char *string_data = malloc(string_length + 1);
    if (fread(string_data, sizeof(char), string_length, file_pointer) != string_length) {
        LOG_ERROR("Failed to read string data from binary scene file");
        free(string_data);
        return NULL; // Return NULL if string data cannot be read
    }
    string_data[string_length] = '\0'; // Null-terminate the string
    return string_data;
}

static bool read_bytes(FILE *file_pointer, void *destination_buffer, size_t size) {
    if (fread(destination_buffer, 1, size, file_pointer) != size) {
        LOG_ERROR("Failed to read bytes from binary scene file");
        return false; // Return false if bytes cannot be read
    }
    return true;
}

static int register_material(Scene *scene, Material material) {
    if (scene->asset_library.material_count >= MAX_MATERIALS) {
        LOG_WARNING("No room to register new material; Registered material count is %d", scene->asset_library.material_count);
        return -1; // No available slot for the material
    }
    int material_id = scene->asset_library.material_count;
    material.id = material_id; // Set the material ID before registering it
    LOG_DEBUG("Registering material with ID %d", material_id);
    scene->asset_library.materials[material_id] = material;
    scene->asset_library.material_count++;
    return material_id;
}

static int register_mesh(Scene *scene, Mesh3D mesh) {
    if (scene->asset_library.mesh_count >= MAX_MESHES) {
        LOG_WARNING("No room to register new mesh; Registered mesh count is %d", scene->asset_library.mesh_count);
        return -1; // No available slot for the mesh
    }
    int mesh_id = scene->asset_library.mesh_count;
    LOG_DEBUG("Registering mesh with ID %d", mesh_id);
    scene->asset_library.meshes[mesh_id] = mesh;
    scene->asset_library.meshes[mesh_id].id = mesh_id;
    scene->asset_library.mesh_count++;
    return mesh_id;
}

static int get_material_id_by_name(Scene *scene, const char *name) {
    for (uint32_t i = 0; i < scene->asset_library.material_count; i++) {
        if (strcmp(scene->asset_library.materials[i].name, name) == 0) {
            return scene->asset_library.materials[i].id;
        }
    }
    LOG_WARNING("Could not find material with name '%s'", name);
    return -1; // Material not found
}

int get_mesh_id_by_name(Scene *scene, const char *name) {
    for (uint32_t i = 0; i < scene->asset_library.mesh_count; i++) {
        if (strcmp(scene->asset_library.meshes[i].name, name) == 0) {
            return scene->asset_library.meshes[i].id;
        }
    }
    LOG_WARNING("Could not find mesh with name '%s'", name);
    return -1; // Mesh not found
}

Scene *load_scene_from_binary(const char *filename, Scene *scene) {
    LOG_DEBUG("Begin binary scene loading function");
    LOG_DEBUG("Opening scene file: %s", filename);
    FILE *file_pointer = fopen(filename, "rb");
    if (!file_pointer) {
        LOG_ERROR("Failed to open scene file: %s", filename);
        return scene; // Return empty scene if file cannot be opened
    }
    LOG_DEBUG("Reading scene file header");
    SceneFileHeader loaded_header = {0};
    read_bytes(file_pointer, &loaded_header.magic, sizeof(uint32_t));
    read_bytes(file_pointer, &loaded_header.version, sizeof(uint32_t));
    read_bytes(file_pointer, &loaded_header.material_count, sizeof(uint32_t));
    read_bytes(file_pointer, &loaded_header.mesh_count, sizeof(uint32_t));
    read_bytes(file_pointer, &loaded_header.entity_count, sizeof(uint32_t));
    LOG_DEBUG("Scene file header: magic=0x%X, version=%d, material_count=%d, mesh_count=%d, entity_count=%d", loaded_header.magic, loaded_header.version, loaded_header.material_count, loaded_header.mesh_count, loaded_header.entity_count);

    LOG_DEBUG("Reading directional light data");
    SceneFileDirectionalLight loaded_directional_light = {0};
    read_bytes(file_pointer, &loaded_directional_light.direction, sizeof(Vector3f));
    read_bytes(file_pointer, &loaded_directional_light.color, sizeof(Vector3f));
    read_bytes(file_pointer, &loaded_directional_light.intensity, sizeof(float));
    read_bytes(file_pointer, &loaded_directional_light.ambient_intensity, sizeof(float));
    LOG_DEBUG("Loaded directional light: direction=(%f, %f, %f), color=(%f, %f, %f), intensity=%f, ambient_intensity=%f", loaded_directional_light.direction.x, loaded_directional_light.direction.y, loaded_directional_light.direction.z, loaded_directional_light.color.x, loaded_directional_light.color.y, loaded_directional_light.color.z, loaded_directional_light.intensity, loaded_directional_light.ambient_intensity);

    scene->directional_light = (DirectionalLight){
        .direction = loaded_directional_light.direction,
        .color.r = loaded_directional_light.color.x,
        .color.g = loaded_directional_light.color.y,
        .color.b = loaded_directional_light.color.z,
        .intensity = loaded_directional_light.intensity,
        .ambient_intensity = loaded_directional_light.ambient_intensity
    };

    LOG_DEBUG("Reading virtual camera data");
    SceneFileCamera loaded_camera = {0};
    read_bytes(file_pointer, &loaded_camera.position, sizeof(Vector3f));
    read_bytes(file_pointer, &loaded_camera.rotation, sizeof(Vector3f));
    read_bytes(file_pointer, &loaded_camera.look_target, sizeof(Vector3f));
    read_bytes(file_pointer, &loaded_camera.aspect_ratio, sizeof(float));
    read_bytes(file_pointer, &loaded_camera.field_of_view, sizeof(float));
    read_bytes(file_pointer, &loaded_camera.near_plane, sizeof(float));
    read_bytes(file_pointer, &loaded_camera.far_plane, sizeof(float));
    LOG_DEBUG("Loaded virtual camera: position=(%f, %f, %f), rotation=(%f, %f, %f), look_target=(%f, %f, %f), aspect_ratio=%f, field_of_view=%f, near_plane=%f, far_plane=%f", loaded_camera.position.x, loaded_camera.position.y, loaded_camera.position.z, loaded_camera.rotation.x, loaded_camera.rotation.y, loaded_camera.rotation.z, loaded_camera.look_target.x, loaded_camera.look_target.y, loaded_camera.look_target.z, loaded_camera.aspect_ratio, loaded_camera.field_of_view, loaded_camera.near_plane, loaded_camera.far_plane);

    scene->virtual_camera = initialize_virtual_camera(loaded_camera.aspect_ratio, loaded_camera.field_of_view, loaded_camera.near_plane, loaded_camera.far_plane, loaded_camera.position, loaded_camera.rotation, loaded_camera.look_target);

    LOG_DEBUG("Allocating memory for materials and meshes");
    scene->asset_library.materials = malloc(sizeof(Material) * loaded_header.material_count);
    scene->asset_library.meshes = malloc(sizeof(Mesh3D) * loaded_header.mesh_count);

    LOG_DEBUG("Reading materials data");
    SceneFileMaterial *loaded_materials = malloc(sizeof(SceneFileMaterial) * loaded_header.material_count);
    for (uint32_t i = 0; i < loaded_header.material_count; i++) {
        LOG_DEBUG("Reading material %d", i);
        SceneFileMaterial loaded_material = {0};
        read_bytes(file_pointer, &loaded_material.shading_model, sizeof(ShadingModel));
        loaded_material.material_name = read_packed_string(file_pointer);
        loaded_material.file_path = read_packed_string(file_pointer);
        loaded_materials[i] = loaded_material;
        LOG_DEBUG("Loaded material %d: name='%s', file_path='%s'", i, loaded_material.material_name, loaded_material.file_path);
    }

    for (uint32_t i = 0; i < loaded_header.material_count; i++) {
        Material material = load_material_from_mtl(loaded_materials[i].file_path, loaded_materials[i].shading_model);
        register_material(scene, material);
        LOG_DEBUG("Registered material '%s' with shading model %d - got ID: %d", material.name, material.shading_model, get_material_id_by_name(scene, material.name));
    }
    free(loaded_materials);

    LOG_DEBUG("Reading meshes data");
    SceneFileMesh *loaded_meshes = malloc(sizeof(SceneFileMesh) * loaded_header.mesh_count);
    for (uint32_t i = 0; i < loaded_header.mesh_count; i++) {
        LOG_DEBUG("Reading mesh %d", i);
        SceneFileMesh loaded_mesh = {0};
        loaded_mesh.mesh_name = read_packed_string(file_pointer);
        loaded_mesh.material_name = read_packed_string(file_pointer);
        loaded_mesh.file_path = read_packed_string(file_pointer);
        loaded_meshes[i] = loaded_mesh;
        LOG_DEBUG("Loaded mesh %d: name='%s', material='%s', file_path='%s'", i, loaded_mesh.mesh_name, loaded_mesh.material_name, loaded_mesh.file_path);
    }

    for (uint32_t i = 0; i < loaded_header.mesh_count; i++) {
        Mesh3D mesh = load_obj(loaded_meshes[i].file_path);
        mesh.material_id = get_material_id_by_name(scene, loaded_meshes[i].material_name);
        LOG_DEBUG("Loaded mesh '%s' uses material '%s' with ID %d", loaded_meshes[i].mesh_name, loaded_meshes[i].material_name, mesh.material_id);
        register_mesh(scene, mesh);
    }
    free(loaded_meshes);

    LOG_DEBUG("Reading entities data");
    SceneFileEntity *loaded_entities = malloc(sizeof(SceneFileEntity) * loaded_header.entity_count);
    for (uint32_t i = 0; i < loaded_header.entity_count; i++) {
        LOG_DEBUG("Reading entity %d", i);
        SceneFileEntity loaded_entity = {0};
        loaded_entity.entity_name = read_packed_string(file_pointer);
        read_bytes(file_pointer, &loaded_entity.component_count, sizeof(uint32_t));
        loaded_entity.components = malloc(sizeof(*loaded_entity.components) * loaded_entity.component_count);
        for (uint32_t j = 0; j < loaded_entity.component_count; j++) {
            LOG_DEBUG("Reading component %d for entity %d", j, i);
            loaded_entity.components[j].component_name = read_packed_string(file_pointer);
            read_bytes(file_pointer, &loaded_entity.components[j].data_size, sizeof(uint32_t));
            loaded_entity.components[j].data = malloc(loaded_entity.components[j].data_size);
            read_bytes(file_pointer, loaded_entity.components[j].data, loaded_entity.components[j].data_size);
            LOG_DEBUG("Loaded component %d for entity %d: name='%s', data_size=%d", j, i, loaded_entity.components[j].component_name, loaded_entity.components[j].data_size);
        }
        loaded_entities[i] = loaded_entity;
        LOG_DEBUG("Loaded entity %d: name='%s', component_count=%d", i, loaded_entity.entity_name, loaded_entity.component_count);
    }

    LOG_DEBUG("Registering entities and their components");
    for (uint32_t i = 0; i < loaded_header.entity_count; i++) {
        Entity entity_id = register_entity(scene, loaded_entities[i].entity_name);
        LOG_DEBUG("Registered entity %d with ID %d", i, entity_id);
        for (uint32_t j = 0; j < loaded_entities[i].component_count; j++) {
            int component_id = -1;
            for (int k = 0; k < scene->registered_component_count; k++) {
                if (strcmp(scene->component_array[k].name, loaded_entities[i].components[j].component_name) == 0) {
                    component_id = k;
                    break;
                }
            }
            if (component_id == -1) {
                LOG_WARNING("Component '%s' for entity '%s' is not registered in the scene, skipping it", loaded_entities[i].components[j].component_name, loaded_entities[i].entity_name);
                continue; // Component not registered, skip it
            }
            char zeroed_data[scene->component_array[component_id].size];
            memset(zeroed_data, 0, scene->component_array[component_id].size);
            add_component(scene, entity_id, component_id, zeroed_data); // Add the component to the entity with zeroed data first to ensure the archetype is set up correctly
            scene->component_array[component_id].parser(scene, entity_id, component_id, loaded_entities[i].components[j].data);
            LOG_DEBUG("Parsed component %d for entity %d", component_id, entity_id);
        }
        free(loaded_entities[i].entity_name);
        for (uint32_t j = 0; j < loaded_entities[i].component_count; j++) {
            free(loaded_entities[i].components[j].component_name); // Free the component name string
            free(loaded_entities[i].components[j].data);
        }
        free(loaded_entities[i].components); // Free the components array for this entity
    }
    free(loaded_entities); // Free the loaded entities array after processing

    fclose(file_pointer);

    return scene;
}

void destroy_scene(Scene *scene) {
    // Free materials
    for (uint32_t i = 0; i < scene->asset_library.material_count; i++) {
        free(scene->asset_library.materials[i].diffuse_texture);
        free(scene->asset_library.materials[i].phong_parameters);
        free(scene->asset_library.materials[i].pbr_parameters);
    }
    free(scene->asset_library.materials);

    // Free meshes
    for (uint32_t i = 0; i < scene->asset_library.mesh_count; i++) {
        free(scene->asset_library.meshes[i].triangles);
        free(scene->asset_library.meshes[i].face_normals);
        free(scene->asset_library.meshes[i].vertex_normals);
        free(scene->asset_library.meshes[i].uv_coordinates);
    }
    free(scene->asset_library.meshes);

    // Free archetype columns
    for (int i = 0; i < scene->registered_archetype_count; i++) {
        Archetype *archetype = &scene->archetypes[i];
        for (int j = 0; j < archetype->column_count; j++) {
            free(archetype->columns[j].component_structures);
        }
        free(archetype->columns);
        free(archetype->component_to_column_map);
        free(archetype->entity_ids);
    }
}
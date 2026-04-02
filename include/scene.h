#pragma once

#include "types.h"

#include "virtual_camera.h"
#include "asset_library.h"

#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 32
#define MAX_COMPONENTS 24
#define MAX_ARCHETYPES 32

#define MAX_MATERIALS 256
#define MAX_MESHES 256

/**
 * @brief A structure representing a scene, including its entities and components.
 */
typedef struct Scene {
    EntityNameToIDMap entity_name_to_id_map[MAX_ENTITIES]; // 69632 bytes
    EntityRecord entity_records[MAX_ENTITIES]; // 8192 bytes
    BitmapFont bitmap_font; // 4104 bytes
    Archetype archetypes[MAX_ARCHETYPES]; // 1536 bytes
    ComponentArray component_array[MAX_COMPONENTS]; // 1440 bytes
    System systems[MAX_SYSTEMS]; // 512 bytes
    VirtualCamera virtual_camera; // 80 bytes
    DirectionalLight directional_light; // 24 bytes
    AssetLibrary asset_library; // 16 bytes
    int registered_entity_count; // 4 bytes
    int registered_component_count; // 4 bytes
    int registered_system_count; // 4 bytes
    int registered_archetype_count; // 4 bytes
    EntityManager entity_manager; // 4 bytes
} Scene;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t material_count;
    uint32_t mesh_count;
    uint32_t entity_count;
} SceneFileHeader;

typedef struct {
    Vector3f direction;
    Vector3f color;
    float intensity;
    float ambient_intensity;
} SceneFileDirectionalLight;

typedef struct {
    Vector3f position;
    Vector3f rotation;
    Vector3f look_target;
    float aspect_ratio;
    float field_of_view;
    float near_plane;
    float far_plane;
} SceneFileCamera;

typedef struct {
    char *material_name;
    char *mesh_name;
    char *file_path;
} SceneFileMesh;

typedef struct {
    ShadingModel shading_model;
    char *material_name;
    char *file_path;
} SceneFileMaterial;

typedef struct {
    char *entity_name;
    uint32_t component_count;
    struct {
        char *component_name;
        uint32_t data_size;
        void *data;
    } *components;
} SceneFileEntity;

Scene initialize_scene();
int get_mesh_id_by_name(Scene *scene, const char *name);
Scene *load_scene_from_binary(const char *filename, Scene *scene);
void destroy_scene(Scene *scene);
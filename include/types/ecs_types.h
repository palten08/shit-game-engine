#pragma once

#include <stddef.h>

#include "vector_types.h"
#include "geometry_types.h"
#include "parson.h"


/** @defgroup ECS Entity Component System Structures
 *  @{
 */

/**
 * @brief A forward declaration of the Scene structure.
 */
typedef struct Scene Scene;

typedef struct AppContext AppContext;

/**
 * @brief A type representing an entity in the ECS.
 * 
 * 4 bytes
 */
typedef int Entity;

/**
 * @brief A structure representing an entity manager, responsible for managing entity IDs.
 * 
 * 4 bytes
 */
typedef struct {
    int next_id; // 4 bytes
} EntityManager;

/**
 * @brief A structure representing a transform component that game projects can associate with an entity
 * 
 * 100 bytes
 */
typedef struct {
    Matrix4 model_matrix; // 64 bytes
    Vector3f position; // 12 bytes
    Vector3f rotation; // 12 bytes
    Vector3f scale;    // 12 bytes
} TransformComponent;

/**
 * @brief A structure representing a mesh component that game projects can associate with an entity
 * 
 * 12 bytes
 */
typedef struct {
    Triangle3D *triangles; // 8 bytes
    int triangle_count; // 4 bytes
} MeshComponent;

/**
 * @brief A function pointer type representing the function that will be used to parse a component from JSON data
 * 
 * 8 bytes
 */
typedef void (*ComponentParserFunction)(Scene *scene, Entity entity, int component_id, JSON_Object *json);

/**
 * @brief A structure representing a component array
 * 
 * 60 bytes
 */
typedef struct {
    char name[32]; // 32 bytes
    void *data; // 8 bytes
    size_t size; // 8 bytes
    ComponentParserFunction parser; // 8 bytes
    int count; // 4 bytes
} ComponentArray;

/**
 * @brief A function pointer type representing the function that systems will use to process the entities that have the components they are concerned with
 * 
 * 8 bytes
 */
typedef void (*SystemFunction)(Scene *scene, AppContext *app_context);

/**
 * @brief A structure representing a system, which processes entities that have specific components
 * 
 * 16 bytes
 */
typedef struct {
    uint64_t required_components; // 8 bytes
    SystemFunction function; // 8 bytes
} System;

/**
 * @brief A structure representing an array of Systems
 * 
 * 20 bytes
 */
typedef struct {
    void *data; // 8 bytes
    size_t size; // 8 bytes
    int count; // 4 bytes
} SystemArray;

/** IDs for built-in engine component types */
extern int TRANSFORM;
extern int MESH;

 /** @} */ // End of ECS group
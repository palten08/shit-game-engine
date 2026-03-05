#pragma once

#include <stddef.h>

#include "vector_types.h"
#include "geometry_types.h"
#include "../scene.h"

/** @defgroup ECS Entity Component System Structures
 *  @{
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
 * @brief A structure representing a component array
 * 
 * 20 bytes
 */
typedef struct {
    void *data; // 8 bytes
    size_t size; // 8 bytes
    int count; // 4 bytes
} ComponentArray;

/**
 * @brief A function pointer type representing the function that systems will use to process the entities that have the components they are concerned with
 * 
 * 8 bytes
 */
typedef void (*SystemFunction)(Scene *scene, float delta_time);

/**
 * @brief A structure representing a system, which processes entities that have specific components
 * 
 * 16 bytes
 */
typedef struct {
    uint64_t required_components; // 8 bytes
    SystemFunction update; // 8 bytes
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

 /** @} */ // End of ECS group
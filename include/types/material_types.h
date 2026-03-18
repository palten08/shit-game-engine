#pragma once

#include "vector_types.h"

/** @defgroup Materials Material System Structures
 *  @{
 */

typedef enum {
    SHADING_FLAT,
    SHADING_PHONG,
    SHADING_PBR
} ShadingModel;

typedef struct {
    uint32_t *pixels; // 8 bytes
    int width; // 4 bytes
    int height; // 4 bytes
} Texture;

typedef struct {
    RGBVector3f ambient_color; // 12 bytes
    RGBVector3f specular_color; // 12 bytes
    float specular_exponent; // 4 bytes
} PhongParameters;

typedef struct {
    float roughness; // 4 bytes
    float metallic; // 4 bytes
    float sheen; // 4 bytes
    float clearcoat; // 4 bytes
    float clearcoat_roughness; // 4 bytes
    float anisotropy_strength; // 4 bytes
    float anisotropy_rotation; // 4 bytes
} PBRParameters;

/**
 * @brief A structure representing a material, including its color properties, texture maps, and shading model.
 * 
 * 128 bytes
 */
typedef struct {
    char name[64]; // 64 bytes
    RGBVector3f diffuse_color; // 12 bytes
    RGBVector3f emissive_color; // 12 bytes
    Texture *diffuse_texture; // 8 bytes
    PhongParameters *phong_parameters; // 8 bytes
    PBRParameters *pbr_parameters; // 8 bytes
    float index_of_refraction; // 4 bytes
    float dissolve; // 4 bytes
    ShadingModel shading_model; // 4 bytes
    int id; // 4 bytes
} Material;

/** @} */ // End of Materials group
#include <stdio.h>

#include "../include/materials.h"
#include "../include/types.h"
#include "../include/logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

Material load_material_from_mtl(const char *file_name, ShadingModel shading_model) {
    LOG_DEBUG("Loading MTL file: %s", file_name);
    Material loaded_material = {0};

    char file_data[256];
    FILE *file_pointer;
    file_pointer = fopen(file_name, "r");
    if (!file_pointer) {
        LOG_ERROR("Error: Failed to open MTL file: %s", file_name);
        return loaded_material;
    }
    if (shading_model == SHADING_PHONG) {
        LOG_DEBUG("Using Phong shading model for material: %s", file_name);
        loaded_material.shading_model = SHADING_PHONG;
        loaded_material.phong_parameters = malloc(sizeof(PhongParameters));
        memset(loaded_material.phong_parameters, 0, sizeof(PhongParameters));
    } else if (shading_model == SHADING_PBR) {
        LOG_DEBUG("Using PBR shading model for material: %s", file_name);
        loaded_material.shading_model = SHADING_PBR;
        loaded_material.pbr_parameters = malloc(sizeof(PBRParameters));
        memset(loaded_material.pbr_parameters, 0, sizeof(PBRParameters));
    } else {
        LOG_DEBUG("Using flat shading model for material: %s", file_name);
        loaded_material.shading_model = SHADING_FLAT;
    }
    while (fgets(file_data, sizeof(file_data), file_pointer) != NULL) {
        char* file_data_copy = strdup(file_data);
        char* line_token = strtok(file_data_copy, " ");
        if (line_token == NULL) {
            free(file_data_copy);
            continue;
        }
        if (strcmp(line_token, "newmtl") == 0) {
            sscanf(file_data, "newmtl %s", loaded_material.name);
        } else if (strcmp(line_token, "Kd") == 0) {
            sscanf(file_data, "Kd %f %f %f", &loaded_material.diffuse_color.r, &loaded_material.diffuse_color.g, &loaded_material.diffuse_color.b);
        } else if (strcmp(line_token, "Ke") == 0) {
            sscanf(file_data, "Ke %f %f %f", &loaded_material.emissive_color.r, &loaded_material.emissive_color.g, &loaded_material.emissive_color.b);
        } else if (strcmp(line_token, "Ni") == 0) {
            sscanf(file_data, "Ni %f", &loaded_material.index_of_refraction);
        } else if (strcmp(line_token, "d") == 0) {
            sscanf(file_data, "d %f", &loaded_material.dissolve);
        }

        switch (shading_model) {
            case SHADING_FLAT:
                break;
            case SHADING_PHONG:
                if (strcmp(line_token, "Ka") == 0) {
                    sscanf(file_data, "Ka %f %f %f", &loaded_material.phong_parameters->ambient_color.r, &loaded_material.phong_parameters->ambient_color.g, &loaded_material.phong_parameters->ambient_color.b);
                } else if (strcmp(line_token, "Ks") == 0) {
                    sscanf(file_data, "Ks %f %f %f", &loaded_material.phong_parameters->specular_color.r, &loaded_material.phong_parameters->specular_color.g, &loaded_material.phong_parameters->specular_color.b);
                } else if (strcmp(line_token, "Ns") == 0) {
                    sscanf(file_data, "Ns %f", &loaded_material.phong_parameters->specular_exponent);
                }
                break;
            case SHADING_PBR:
                if (strcmp(line_token, "Pr") == 0) {
                    sscanf(file_data, "Pr %f", &loaded_material.pbr_parameters->roughness);
                } else if (strcmp(line_token, "Pm") == 0) {
                    sscanf(file_data, "Pm %f", &loaded_material.pbr_parameters->metallic);
                } else if (strcmp(line_token, "Ps") == 0) {
                    sscanf(file_data, "Ps %f", &loaded_material.pbr_parameters->sheen);
                } else if (strcmp(line_token, "Pc") == 0) {
                    sscanf(file_data, "Pc %f", &loaded_material.pbr_parameters->clearcoat);
                } else if (strcmp(line_token, "Pcr") == 0) {
                    sscanf(file_data, "Pcr %f", &loaded_material.pbr_parameters->clearcoat_roughness);
                } else if (strcmp(line_token, "Pas") == 0) {
                    sscanf(file_data, "Pas %f", &loaded_material.pbr_parameters->anisotropy_strength);
                } else if (strcmp(line_token, "Par") == 0) {
                    sscanf(file_data, "Par %f", &loaded_material.pbr_parameters->anisotropy_rotation);
                }
                break;
        }

        if (strcmp(line_token, "map_Kd") == 0) {
            char texture_file_name[128];
            sscanf(file_data, "map_Kd %s", texture_file_name);
            char texture_file_path[256];
            strncpy(texture_file_path, file_name, sizeof(texture_file_path));
            char *last_slash = strrchr(texture_file_path, '/');
            if (last_slash) {
                strcpy(last_slash + 1, texture_file_name);
            } else {
                strncpy(texture_file_path, texture_file_name, sizeof(texture_file_path));
            }
            loaded_material.diffuse_texture = malloc(sizeof(Texture));

            int width, height, channels;
            unsigned char *image_data = stbi_load(texture_file_path, &width, &height, &channels, 4);
            if (!image_data) {
                LOG_ERROR("Error: Failed to load texture image: %s", texture_file_path);
                free(file_data_copy);
                free(loaded_material.diffuse_texture);
                loaded_material.diffuse_texture = NULL;
                continue;
            }
            loaded_material.diffuse_texture->width = width;
            loaded_material.diffuse_texture->height = height;
            loaded_material.diffuse_texture->pixels = (uint32_t*)image_data;
        }

    }

    LOG_DEBUG("Loaded material: %s", loaded_material.name);
    LOG_DEBUG("Diffuse color: %f, %f, %f", loaded_material.diffuse_color.r, loaded_material.diffuse_color.g, loaded_material.diffuse_color.b);
    LOG_DEBUG("Emissive color: %f, %f, %f", loaded_material.emissive_color.r, loaded_material.emissive_color.g, loaded_material.emissive_color.b);
    LOG_DEBUG("Index of refraction: %f", loaded_material.index_of_refraction);
    LOG_DEBUG("Dissolve: %f", loaded_material.dissolve);
    if (loaded_material.shading_model == SHADING_PHONG) {
        LOG_DEBUG("Phong ambient color: %f, %f, %f", loaded_material.phong_parameters->ambient_color.r, loaded_material.phong_parameters->ambient_color.g, loaded_material.phong_parameters->ambient_color.b);
        LOG_DEBUG("Phong specular color: %f, %f, %f", loaded_material.phong_parameters->specular_color.r, loaded_material.phong_parameters->specular_color.g, loaded_material.phong_parameters->specular_color.b);
        LOG_DEBUG("Phong specular exponent: %f", loaded_material.phong_parameters->specular_exponent);
    } else if (loaded_material.shading_model == SHADING_PBR) {
        LOG_DEBUG("PBR roughness: %f", loaded_material.pbr_parameters->roughness);
        LOG_DEBUG("PBR metallic: %f", loaded_material.pbr_parameters->metallic);
        LOG_DEBUG("PBR sheen: %f", loaded_material.pbr_parameters->sheen);
        LOG_DEBUG("PBR clearcoat: %f", loaded_material.pbr_parameters->clearcoat);
        LOG_DEBUG("PBR clearcoat roughness: %f", loaded_material.pbr_parameters->clearcoat_roughness);
        LOG_DEBUG("PBR anisotropy strength: %f", loaded_material.pbr_parameters->anisotropy_strength);
        LOG_DEBUG("PBR anisotropy rotation: %f", loaded_material.pbr_parameters->anisotropy_rotation);
    }
    return loaded_material;
}
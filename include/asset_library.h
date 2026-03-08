#pragma once

#include "types.h"

typedef struct {
    Mesh3D *meshes; // 8 bytes
    uint32_t mesh_count; // 4 bytes
} AssetLibrary;
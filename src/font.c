#include <stdio.h>
#include <stdlib.h>

#include "../include/font.h"
#include "../include/stb_image.h"
#include "../include/logging.h"

BitmapFont load_bitmap_font(const char *atlas_image_file_path, const char *fnt_file_path) {
    BitmapFont loaded_font = {0};
    loaded_font.pixel_data = stbi_load(atlas_image_file_path, &loaded_font.image_dimensions.x, &loaded_font.image_dimensions.y, NULL, 4); // Load with 4 channels (RGBA)
    if (!loaded_font.pixel_data) {
        LOG_ERROR("Failed to load bitmap font from path: %s", atlas_image_file_path);
        return loaded_font;
    }
    LOG_DEBUG("Loaded bitmap font atlas image: %s (Dimensions: %d x %d)", atlas_image_file_path, loaded_font.image_dimensions.x, loaded_font.image_dimensions.y);
    LOG_DEBUG("Pixel data: %p", loaded_font.pixel_data);
    char fnt_file_data[512];
    FILE *fnt_file_pointer;
    fnt_file_pointer = fopen(fnt_file_path, "r");
    if (!fnt_file_pointer) {
        LOG_ERROR("Failed to open font file: %s", fnt_file_path);
        return loaded_font;
    }
    while (fgets(fnt_file_data, sizeof(fnt_file_data), fnt_file_pointer) != NULL) {
        char *fnt_file_data_copy = strdup(fnt_file_data);
        char *line_token = strtok(fnt_file_data_copy, " ");
        if (line_token == NULL) {
            free(fnt_file_data_copy);
            continue;
        }
        if (strcmp(line_token, "common") == 0) {
            sscanf(fnt_file_data, "common lineHeight=%d", &loaded_font.line_height);
        } else if (strcmp(line_token, "char") == 0) {
            int character_id;
            sscanf(fnt_file_data, "char id=%d", &character_id);
            if (character_id < 0 || character_id >= 128) {
                LOG_WARNING("Warning: Character ID %d in font file %s is out of bounds and will be skipped.", character_id, fnt_file_path);
                free(fnt_file_data_copy);
                continue;
            }
            sscanf(fnt_file_data, "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d", &character_id, &loaded_font.characters[character_id].atlas_position.x, &loaded_font.characters[character_id].atlas_position.y, &loaded_font.characters[character_id].atlas_size.x, &loaded_font.characters[character_id].atlas_size.y, &loaded_font.characters[character_id].cursor_offset.x, &loaded_font.characters[character_id].cursor_offset.y, &loaded_font.characters[character_id].xadvance);
        }
        free(fnt_file_data_copy);
    }
    fclose(fnt_file_pointer);
    return loaded_font;
}
#include <math.h>
#include <stdarg.h>

#include "../include/ui.h"
#include "../include/types.h"
#include "../include/app.h"
#include "../include/utils.h"
#include "../include/stb_image.h"

void ui_draw_rectangle(UIRectangle rectangle, AppContext *app_context) {
    int start_x = fmax(0, rectangle.position.x);
    int end_x = fmin(rectangle.position.x + rectangle.size.x, app_context->window_resolution.x);
    int start_y = fmax(0, rectangle.position.y);
    int end_y = fmin(rectangle.position.y + rectangle.size.y, app_context->window_resolution.y);

    for (int x = start_x; x < end_x; x++) {
        for (int y = start_y; y < end_y; y++) {
            draw_pixel_at_coordinates(app_context, x, y, rectangle.color);
        }
    }
}

void ui_draw_outline(Vector2i screen_position, Vector2i size, int thickness, RGBVector3f color, AppContext *app_context) {
    ui_draw_rectangle((UIRectangle){.position = screen_position, .size = (Vector2i){size.x, thickness}, .color = color}, app_context);
    ui_draw_rectangle((UIRectangle){.position = {screen_position.x, screen_position.y + size.y - thickness}, .size = {size.x, thickness}, .color = color}, app_context);
    ui_draw_rectangle((UIRectangle){.position = screen_position, .size = {thickness, size.y}, .color = color}, app_context);
    ui_draw_rectangle((UIRectangle){.position = {screen_position.x + size.x - thickness, screen_position.y}, .size = {thickness, size.y}, .color = color}, app_context);
}

void ui_draw_character(BitmapFont *font, char character, Vector2i screen_position, RGBVector3f color, AppContext *app_context, int size_multiplier) {
    FontCharacter font_character = font->characters[(int)character];
    for (int x = 0; x < font_character.atlas_size.x; x++) {
        for (int y = 0; y < font_character.atlas_size.y; y++) {
            int pixel_index = ((font_character.atlas_position.y + y) * font->image_dimensions.x + (font_character.atlas_position.x + x)) * 4;
            uint8_t r = ((uint8_t*)font->pixel_data)[pixel_index];
            uint8_t g = ((uint8_t*)font->pixel_data)[pixel_index + 1];
            uint8_t b = ((uint8_t*)font->pixel_data)[pixel_index + 2];
            if (r > 0 || g > 0 || b > 0) {
                for (int i = 0; i < size_multiplier; i++) {
                    for (int j = 0; j < size_multiplier; j++) {
                        draw_pixel_at_coordinates(app_context, screen_position.x + (x + font_character.cursor_offset.x) * size_multiplier + i, screen_position.y + (y + font_character.cursor_offset.y) * size_multiplier + j, color);
                    }
                }
            }
        }
    }
}

void ui_draw_string(BitmapFont *font, const char *string, Vector2i screen_position, RGBVector3f color, AppContext *app_context, int size_multiplier) {
    Vector2i cursor_position = screen_position;
    size_t string_length = strlen(string);
    for (size_t i = 0; i < string_length; i++) {
        char character = string[i];
        if (character == '\n') {
            cursor_position.x = screen_position.x;
            cursor_position.y += font->line_height * size_multiplier;
        } else {
            ui_draw_character(font, character, cursor_position, color, app_context, size_multiplier);
            cursor_position.x += font->characters[(int)character].xadvance * size_multiplier;
        }
    }
}

void ui_draw_string_formatted(BitmapFont *font, Vector2i screen_position, RGBVector3f color, AppContext *app_context, int size_multiplier, const char *format, ...) {
    char formatted_string[256];
    va_list args;
    va_start(args, format);
    vsnprintf(formatted_string, sizeof(formatted_string), format, args);
    va_end(args);
    ui_draw_string(font, formatted_string, screen_position, color, app_context, size_multiplier);
}

void draw_ui(AppContext *app_context, Scene *scene) {
    ui_draw_rectangle((UIRectangle){.position = {10, 10}, .size = {400, 100}, .color = {0.0f, 0.5f, 1.0f}, .border_thickness = 2}, app_context);
    ui_draw_outline((Vector2i){10, 10}, (Vector2i){400, 100}, 2, (RGBVector3f){1.0f, 1.0f, 1.0f}, app_context);
    double instantaneous_fps = 1.0 / app_context->delta_time;
    ui_draw_string_formatted(&scene->bitmap_font, (Vector2i){20, 20}, (RGBVector3f){1.0f, 1.0f, 1.0f}, app_context, 2, "FPS: %.2f", instantaneous_fps);
}
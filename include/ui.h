#pragma once

#include "types.h"

void ui_draw_rectangle(UIRectangle rectangle, AppContext *app_context);
void ui_draw_outline(Vector2i screen_position, Vector2i size, int thickness, RGBVector3f color, AppContext *app_context);
void ui_draw_character(BitmapFont *font, char character, Vector2i screen_position, RGBVector3f color, AppContext *app_context, int size_multiplier);
void ui_draw_string(BitmapFont *font, const char *string, Vector2i screen_position, RGBVector3f color, AppContext *app_context, int size_multiplier);
void ui_draw_string_formatted(BitmapFont *font, Vector2i screen_position, RGBVector3f color, AppContext *app_context, int size_multiplier, const char *format, ...);
void draw_ui(AppContext *app_context, Scene *scene);
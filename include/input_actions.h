#pragma once

#include <SDL2/SDL.h>
#include "types.h"
#include "app.h"

int register_button_input_action(InputActionMap *input_action_map, const char *name, InputBinding *input_action_bindings, int binding_count);
int register_axis_2d_input_action(InputActionMap *input_action_map, const char *name, AxisBindings positive_x_bindings, AxisBindings negative_x_bindings, AxisBindings positive_y_bindings, AxisBindings negative_y_bindings);
int register_axis_1d_input_action(InputActionMap *input_action_map, const char *name, AxisBindings positive_bindings, AxisBindings negative_bindings);
void update_input_actions(InputAction *actions, int count);

bool is_input_action_held(InputAction *input_action);
bool is_input_action_pressed(InputAction *input_action);
bool is_input_action_released(InputAction *input_action);
Vector2f get_input_action_axis_2d_value(InputAction *input_action);
float get_input_action_axis_1d_value(InputAction *input_action);

Vector2i get_mouse_position(void);
Vector2i get_mouse_delta(void);

int get_input_action_index_by_name(InputActionMap *input_action_map, const char *name);
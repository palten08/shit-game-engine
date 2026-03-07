#include <SDL2/SDL.h>
#include "../include/input_actions.h"
#include "../include/app.h"

/**
 * @brief Registers a new button input action in the provided InputActionMap
 * @param input_action_map The InputActionMap to register the action in
 * @param name The name of the input action
 * @param input_action_bindings An array of InputBinding structs representing the bindings for the action
 * @param binding_count The number of bindings in the input_action_bindings array
 * @return The index of the registered action, or -1 if registration failed
 */
int register_button_input_action(InputActionMap *input_action_map, const char *name, InputBinding *input_action_bindings, int binding_count) {
    for (int i = 0; i < MAX_INPUT_ACTIONS; i++) {
        if (input_action_map->input_actions[i].type == 0) { // Unused slot
            strncpy(input_action_map->input_actions[i].name, name, sizeof(input_action_map->input_actions[i].name) - 1);
            input_action_map->input_actions[i].type = ACTION_TYPE_BUTTON;
            for (int j = 0; j < binding_count && j < 8; j++) {
                input_action_map->input_actions[i].button.bindings[j] = input_action_bindings[j];
            }
            return i; // Return the index of the registered action
        }
    }
    return -1; // No available slot
}

/**
 * @brief Registers a new 2D axis input action in the provided InputActionMap
 * @param input_action_map The InputActionMap to register the action in
 * @param name The name of the input action
 * @param positive_x_bindings An AxisBindings struct containing the positive X bindings for the axis
 * @param negative_x_bindings An AxisBindings struct containing the negative X bindings for the axis
 * @param positive_y_bindings An AxisBindings struct containing the positive Y bindings for the axis
 * @param negative_y_bindings An AxisBindings struct containing the negative Y bindings for the axis
 * @return The index of the registered action, or -1 if registration failed
 */
int register_axis_2d_input_action(InputActionMap *input_action_map, const char *name, AxisBindings positive_x_bindings, AxisBindings negative_x_bindings, AxisBindings positive_y_bindings, AxisBindings negative_y_bindings) {
    for (int i = 0; i < MAX_INPUT_ACTIONS; i++) {
        if (input_action_map->input_actions[i].type == 0) { // Unused slot
            strncpy(input_action_map->input_actions[i].name, name, sizeof(input_action_map->input_actions[i].name) - 1);
            input_action_map->input_actions[i].type = ACTION_TYPE_AXIS_2D;
            for (int j = 0; j < positive_x_bindings.binding_count && j < 4; j++) {
                input_action_map->input_actions[i].axis_2d.positive_x[j] = positive_x_bindings.bindings[j];
            }
            for (int j = 0; j < negative_x_bindings.binding_count && j < 4; j++) {
                input_action_map->input_actions[i].axis_2d.negative_x[j] = negative_x_bindings.bindings[j];
            }
            for (int j = 0; j < positive_y_bindings.binding_count && j < 4; j++) {
                input_action_map->input_actions[i].axis_2d.positive_y[j] = positive_y_bindings.bindings[j];
            }
            for (int j = 0; j < negative_y_bindings.binding_count && j < 4; j++) {
                input_action_map->input_actions[i].axis_2d.negative_y[j] = negative_y_bindings.bindings[j];
            }
            return i; // Return the index of the registered action
        }
    }
    return -1; // No available slot
}

/**
 * @brief Registers a new 1D axis input action in the provided InputActionMap
 * @param input_action_map The InputActionMap to register the action in
 * @param name The name of the input action
 * @param positive_bindings An AxisBindings struct containing the positive bindings for the axis
 * @param negative_bindings An AxisBindings struct containing the negative bindings for the axis
 * @return The index of the registered action, or -1 if registration failed
 */
int register_axis_1d_input_action(InputActionMap *input_action_map, const char *name, AxisBindings positive_bindings, AxisBindings negative_bindings) {
    for (int i = 0; i < MAX_INPUT_ACTIONS; i++) {
        if (input_action_map->input_actions[i].type == 0) { // Unused slot
            strncpy(input_action_map->input_actions[i].name, name, sizeof(input_action_map->input_actions[i].name) - 1);
            input_action_map->input_actions[i].type = ACTION_TYPE_AXIS_1D;
            for (int j = 0; j < positive_bindings.binding_count && j < 4; j++) {
                input_action_map->input_actions[i].axis_1d.positive[j] = positive_bindings.bindings[j];
            }
            for (int j = 0; j < negative_bindings.binding_count && j < 4; j++) {
                input_action_map->input_actions[i].axis_1d.negative[j] = negative_bindings.bindings[j];
            }
            return i; // Return the index of the registered action
        }
    }
    return -1; // No available slot
}

/**
 * @brief Checks if a given input binding is currently active
 * @param binding The InputBinding to check
 * @param keyboard The current state of the keyboard
 * @param mouse The current state of the mouse buttons
 * @return true if the binding is active, false otherwise
 */
static bool is_binding_active(InputBinding *binding, const Uint8 *keyboard, Uint32 mouse) {
    switch (binding->type) {
        case INPUT_KEYBOARD:
            return binding->code != SDL_SCANCODE_UNKNOWN && keyboard[binding->code];
        case INPUT_MOUSE_BUTTON:
            return mouse & SDL_BUTTON(binding->code);
        case INPUT_GAMEPAD_BUTTON:
            return false; // TODO
        default:
            return false;
    }
}

/**
 * @brief Updates the state of all input actions based on the current input state
 * @param actions An array of InputAction to update
 * @param count The number of InputAction in the array
 */
void update_input_actions(InputAction *actions, int count) {
    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
    Uint32 mouse = SDL_GetMouseState(NULL, NULL);

    for (int i = 0; i < count; i++) {
        switch (actions[i].type) {
            case ACTION_TYPE_BUTTON: {
                actions[i].button.previous_state = actions[i].button.current_state;
                actions[i].button.current_state = false;
                for (int j = 0; j < 8; j++) {
                    if (is_binding_active(&actions[i].button.bindings[j], keyboard, mouse)) {
                        actions[i].button.current_state = true;
                        break;
                    }
                }
                break;
            }
            case ACTION_TYPE_AXIS_2D: {
                actions[i].axis_2d.previously_active = actions[i].axis_2d.currently_active;
                float x = 0.0f, y = 0.0f;
                for (int j = 0; j < 4; j++) {
                    if (is_binding_active(&actions[i].axis_2d.positive_x[j], keyboard, mouse)) x += 1.0f;
                    if (is_binding_active(&actions[i].axis_2d.negative_x[j], keyboard, mouse)) x -= 1.0f;
                    if (is_binding_active(&actions[i].axis_2d.positive_y[j], keyboard, mouse)) y += 1.0f;
                    if (is_binding_active(&actions[i].axis_2d.negative_y[j], keyboard, mouse)) y -= 1.0f;
                }
                float len = sqrtf(x * x + y * y);
                if (len > 0.0f) { x /= len; y /= len; }
                actions[i].axis_2d.value = (Vector2f){x, y};
                actions[i].axis_2d.currently_active = (x != 0.0f || y != 0.0f);
                break;
            }
            case ACTION_TYPE_AXIS_1D: {
                actions[i].axis_1d.previously_active = actions[i].axis_1d.currently_active;
                float val = 0.0f;
                for (int j = 0; j < 4; j++) {
                    if (is_binding_active(&actions[i].axis_1d.positive[j], keyboard, mouse)) val += 1.0f;
                    if (is_binding_active(&actions[i].axis_1d.negative[j], keyboard, mouse)) val -= 1.0f;
                }
                actions[i].axis_1d.value = fminf(1.0f, fmaxf(-1.0f, val));
                actions[i].axis_1d.currently_active = (val != 0.0f);
                break;
            }
            default: break;
        }
    }
}


bool is_input_action_held(InputAction *input_action) {
    switch (input_action->type) {
        case ACTION_TYPE_BUTTON:
            return input_action->button.current_state;
        case ACTION_TYPE_AXIS_1D:
            return input_action->axis_1d.currently_active;
        case ACTION_TYPE_AXIS_2D:
            return input_action->axis_2d.currently_active;
        default:
            return false;
    }
}

bool is_input_action_pressed(InputAction *input_action) {
    switch (input_action->type) {
        case ACTION_TYPE_BUTTON:
            return input_action->button.current_state && !input_action->button.previous_state;
        case ACTION_TYPE_AXIS_1D:
            return input_action->axis_1d.currently_active && !input_action->axis_1d.previously_active;
        case ACTION_TYPE_AXIS_2D:
            return input_action->axis_2d.currently_active && !input_action->axis_2d.previously_active;
        default:
            return false;
    }
}

bool is_input_action_released(InputAction *input_action) {
    switch (input_action->type) {
        case ACTION_TYPE_BUTTON:
            return !input_action->button.current_state && input_action->button.previous_state;
        case ACTION_TYPE_AXIS_1D:
            return !input_action->axis_1d.currently_active && input_action->axis_1d.previously_active;
        case ACTION_TYPE_AXIS_2D:
            return !input_action->axis_2d.currently_active && input_action->axis_2d.previously_active;
        default:
            return false;
    }
}

Vector2f get_input_action_axis_2d_value(InputAction *input_action) {
    return input_action->axis_2d.value;
}

float get_input_action_axis_1d_value(InputAction *input_action) {
    return input_action->axis_1d.value;
}

int get_input_action_index_by_name(InputActionMap *input_action_map, const char *name) {
    for (int i = 0; i < MAX_INPUT_ACTIONS; i++) {
        if (input_action_map->input_actions[i].type != 0 && strcmp(input_action_map->input_actions[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

/**
 * @brief Gets the current mouse position
 * @return A Vector2i representing the current mouse position
 */
Vector2i get_mouse_position(void) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return (Vector2i){x, y};
}

/**
 * @brief Gets the relative mouse movement since the last call to this function
 * @return A Vector2i representing the relative mouse movement
 */
Vector2i get_mouse_delta(void) {
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    return (Vector2i){x, y};
}

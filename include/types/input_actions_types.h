#pragma once

#include <SDL2/SDL.h>

/** @defgroup InputActions Input Action Structures
 *  @{
 */

#define MAX_INPUT_ACTIONS 128

typedef struct {
    enum { INPUT_KEYBOARD, INPUT_MOUSE_BUTTON, INPUT_GAMEPAD_BUTTON } type;
    int code;
} InputBinding;

typedef enum {
    ACTION_TYPE_NONE = 0,
    ACTION_TYPE_BUTTON,
    ACTION_TYPE_AXIS_1D,
    ACTION_TYPE_AXIS_2D
} InputActionType;


typedef struct {
    char name[64];
    InputActionType type;

    union {
        struct {
            InputBinding bindings[8];
            bool current_state;
            bool previous_state;
        } button;

        struct {
            InputBinding positive_x[4];
            InputBinding negative_x[4];
            InputBinding positive_y[4];
            InputBinding negative_y[4];
            Vector2f value;
            bool currently_active;
            bool previously_active;
        } axis_2d;

        struct {
            InputBinding positive[4];
            InputBinding negative[4];
            float value;
            bool currently_active;
            bool previously_active;
        } axis_1d;
    };
} InputAction;

/**
 * @brief A structure representing a mapping of input actions
 * 
 * 8960 bytes
 */
typedef struct {
    InputAction input_actions[MAX_INPUT_ACTIONS];
} InputActionMap;

typedef struct {
    InputBinding *bindings;
    int binding_count;
} AxisBindings;

 /** @} */ // End of InputActions group
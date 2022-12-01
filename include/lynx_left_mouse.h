#ifndef LYNX_LEFT_MOUSE_H
#define LYNX_LEFT_MOUSE_H

#include <Mouse.h>
#include "lynx_left_constants.h"
#include "lynx_left_custom_types.h"
#include "lynx_left_activity.h"

#define MOUSE_AXIS_RANGE (12)
#define MOUSE_AXIS_THRESHOLD (MOUSE_AXIS_RANGE / 4)
#define MOUSE_AXIS_CENTER (MOUSE_AXIS_RANGE / 2)

typedef enum {
	DEFAULT_MOUSE_LAYOUT,
    MAX_MOUSE_LAYOUT
} mouse_layout;

typedef struct s_mouse_set {
  mouse_layout id;
  const keypad_array * keypad;
} mouse_set;

void setup_mouse_mode(void);
void end_mouse_mode(void);
void mouse_mode_change(void);
void mouse_press_key(struct s_key_item);
void mouse_release_key(struct s_key_item);
void mouse_joystick_update(joystick_axis, int);
void mouse_joystick_update_both(int, int);

#endif // LYNX_LEFT_MOUSE_H

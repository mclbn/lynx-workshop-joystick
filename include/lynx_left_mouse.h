#ifndef LYNX_LEFT_MOUSE_H
#define LYNX_LEFT_MOUSE_H

#include <Mouse.h>
#include "lynx_left_constants.h"
#include "lynx_left_custom_types.h"
#include "lynx_left_activity.h"

void setup_mouse_mode(void);
void end_mouse_mode(void);
void mouse_mode_change(void);
void mouse_press_key(struct s_key_item key);
void mouse_release_key(struct s_key_item key);
void mouse_joystick_update(joystick_axis axis, int value);

#endif // LYNX_LEFT_MOUSE_H

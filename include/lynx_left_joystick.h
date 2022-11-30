#ifndef LYNX_LEFT_JOYSTICK_H
#define LYNX_LEFT_JOYSTICK_H

#include <Joystick.h>
#include "lynx_left_constants.h"
#include "lynx_left_activity.h"

#define JOYSTICK_BUTTON_NUMBER (17)

void setup_joystick_mode(void);
void end_joystick_mode(void);
void joystick_mode_change(void);
void joystick_press_key(struct s_key_item key);
void joystick_release_key(struct s_key_item key);
void joystick_press_joykey(int value);
void joystick_release_joykey(int value);

#endif // LYNX_LEFT_JOYSTICK_H

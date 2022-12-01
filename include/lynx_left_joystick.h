#ifndef LYNX_LEFT_JOYSTICK_H
#define LYNX_LEFT_JOYSTICK_H

#include <Joystick.h>
#include "lynx_left_constants.h"
#include "lynx_left_custom_types.h"
#include "lynx_left_activity.h"

#define JOYSTICK_BUTTON_NUMBER (17)

typedef enum {
	DEFAULT_JOYSTICK_LAYOUT,
    MAX_JOYSTICK_LAYOUT
} joystick_layout;

typedef struct s_joystick_set {
  joystick_layout id;
  const keypad_array * keypad;
} joystick_set;

void setup_joystick_mode(void);
void end_joystick_mode(void);
void joystick_mode_change(void);
void joystick_press_key(struct s_key_item);
void joystick_release_key(struct s_key_item);
void joystick_joystick_update(joystick_axis , int);
void joystick_joystick_update_both(int , int);

#endif // LYNX_LEFT_JOYSTICK_H

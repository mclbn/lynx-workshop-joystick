#ifndef LYNX_LEFT_KEYBOARD_H_
#define LYNX_LEFT_KEYBOARD_H_

#include <Keyboard.h>
#include "lynx_left_constants.h"
#include "lynx_left_custom_types.h"
#include "lynx_left_activity.h"

#define JOY_VERYLOW (100)
#define JOY_LOW (400)
#define JOY_HIGH (600)
#define JOY_VERYHIGH (900)

typedef enum {
  JOY_STATE_OFF,
  JOY_STATE_LOW,
  JOY_STATE_HIGH
} joy_state;

typedef enum {
	DEFAULT_KEYBOARD_LAYOUT,
	ALTERNATIVE_KEYBOARD_LAYOUT,
	CYBERPUNK_KEYBOARD_LAYOUT,
	PREY_KEYBOARD_LAYOUT,
	STARSECTOR_KEYBOARD_LAYOUT,
	MAX_KEYBOARD_LAYOUT
} keypad_layout;

typedef struct s_keyboard_joystick_layout {
  int up_value;
  int down_value;
  int left_value;
  int right_value;
  int low_mod_value;
  int high_mod_value;
} keyboard_joystick_layout;

typedef struct s_keyboard_set {
  keypad_layout id;
  const keypad_array * keypad;
  const keyboard_joystick_layout * joystick;
} keyboard_set;

void setup_keyboard_mode(void);
void end_keyboard_mode(void);
void keyboard_mode_change(void);
void keyboard_press_key(struct s_key_item key);
void keyboard_release_key(struct s_key_item key);
void keyboard_joystick_update(joystick_axis axis, int value);

#endif // LYNX_LEFT_KEYBOARD_H_

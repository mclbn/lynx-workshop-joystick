#ifndef LYNX_LEFT_KEYBOARD_H_
#define LYNX_LEFT_KEYBOARD_H_

#include <Keyboard.h>
#include "lynx_left_constants.h"
#include "lynx_left_activity.h"

typedef enum {
	DEFAULT_LAYOUT,
	ALTERNATIVE_LAYOUT,
	CYBERPUNK_LAYOUT,
	PREY_LAYOUT,
	STARSECTOR_LAYOUT,
	MAX_LAYOUT
} keypad_layout;


#ifdef JOYSTICK_LAYOUT_H

typedef int keypad_array[ROW_NB][COLUMN_NB];

typedef struct s_keyboard_set {
  keypad_layout id;
  const keypad_array * keypad;
  const joystick_layout * joystick;
} keyboard_set;

#endif

void setup_keyboard_mode(void);
void end_keyboard_mode(void);
void keyboard_mode_change(void);
void keyboard_press_key(struct s_key_item key);
void keyboard_release_key(struct s_key_item key);
void keyboard_press_joykey(int value);
void keyboard_release_joykey(int value);

#endif // LYNX_LEFT_KEYBOARD_H_

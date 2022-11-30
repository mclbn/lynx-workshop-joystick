#ifndef LYNX_LEFT_ACTIVITY_H
#define LYNX_LEFT_ACTIVITY_H

#include <Arduino.h>
#include "lynx_left_constants.h"
#include "lynx_left_keyboard.h"
#include "lynx_left_joystick.h"
#include "lynx_left_mouse.h"

#define DEBOUNCE_DELAY (25)
#define MAX_KEYS (10)
#define HOLD_TIME (500)

#define NO_KEY '\0'
#define NO_CODE (-1)

#define ALT_KEY_CODE (13)
#define ALT_JOY_CODE (14)
#define ALT_MOUSE_CODE (15)

#define JOY_VERYLOW (100)
#define JOY_LOW (400)
#define JOY_HIGH (600)
#define JOY_VERYHIGH (900)

typedef enum {
	KEYBOARD_MODE,
	JOYSTICK_MODE,
	MOUSE_MODE
} input_mode;

typedef enum {
	IDLE,
	PRESSED,
	HELD,
	RELEASED
} key_state;

typedef enum {
	JOY_STATE_OFF,
	JOY_STATE_LOW,
	JOY_STATE_HIGH
} joy_state;

typedef struct s_key_item {
	int8_t code;
	int value;
	key_state state;
	bool state_changed;
	unsigned long hold_timer;
} key_item;

typedef struct s_button_layout {
	int8_t code;
	char const * label; // Not used, but helps mapping new layouts :-)
	int value;
	bool active;
} button_layout;

#ifndef JOYSTICK_LAYOUT_H
#define JOYSTICK_LAYOUT_H

typedef struct s_joystick_layout {
	int up_value;
	int down_value;
	int left_value;
	int right_value;
	int low_mod_value;
	int high_mod_value;
} joystick_layout;

#endif // JOYSTICK_LAYOUT_H

typedef struct s_activity_actions {
  void (*press_button)(key_item);
  void (*release_button)(key_item);
  void (*press_joystick)(int value);
  void (*release_joystick)(int value);
  void (*mode_change)(void);
} activity_actions;

void setup_activity(void);
void handle_activity(void);
void setup_actions(void (*press_button)(key_item),
                   void (*release_button)(key_item),
                   void (*press_joystick)(int),
                   void (*release_joystick)(int),
                   void (*mode_change)(void));

extern button_layout keypad[ROW_NB][COLUMN_NB];
extern joystick_layout joystick;

#endif // LYNX_LEFT_ACTIVITY_H

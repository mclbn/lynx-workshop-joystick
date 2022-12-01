#ifndef LYNX_LEFT_ACTIVITY_H
#define LYNX_LEFT_ACTIVITY_H

#include <Arduino.h>
#include "lynx_left_constants.h"
#include "lynx_left_custom_types.h"
#include "lynx_left_keyboard.h"
#include "lynx_left_joystick.h"
#include "lynx_left_mouse.h"

#define DEBOUNCE_DELAY (25)
#define MAX_KEYS (10)
#define HOLD_TIME (500)

#define NO_CODE (-1)

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

typedef struct s_key_item {
  int8_t code;
  int value;
  key_state state;
  bool state_changed;
  unsigned long hold_timer;
} key_item;

typedef struct s_button_layout {
  int8_t code;
  char const * label; // Not used, but helps mapping new layouts
  int value;
  bool active;
} button_layout;

typedef struct s_activity_actions {
  void (*press_button)(key_item);
  void (*release_button)(key_item);
  void (*joystick_update)(joystick_axis axis, int value);
  void (*mode_change)(void);
} activity_actions;

void setup_activity(void);
void handle_activity(void);
void setup_actions(void (*press_button)(key_item),
                   void (*release_button)(key_item),
                   void (*joystick_update)(joystick_axis, int),
                   void (*mode_change)(void));

extern button_layout keypad[ROW_NB][COLUMN_NB];

#endif // LYNX_LEFT_ACTIVITY_H

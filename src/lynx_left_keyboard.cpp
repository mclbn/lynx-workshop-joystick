#include "lynx_left_keyboard.h"

static bool up_changed = false;
static bool down_changed = false;
static bool left_changed = false;
static bool right_changed = false;
static bool low_mod_active = false;
static bool high_mod_active = false;

static joy_state up_axis = JOY_STATE_OFF;
static joy_state down_axis = JOY_STATE_OFF;
static joy_state left_axis = JOY_STATE_OFF;
static joy_state right_axis = JOY_STATE_OFF;

static keyboard_joystick_layout joystick = {
  NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY
};

static const int default_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_F9, KEY_F5, KEY_F1, KEY_RETURN, KEY_TAB, NO_KEY},
  {NO_KEY, KEY_F10, KEY_F6, KEY_F2, NO_KEY, KEY_LEFT_GUI, NO_KEY},
  {NO_KEY, KEY_F11, KEY_F7, KEY_F3, NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, KEY_F12, KEY_F8, KEY_F4, KEY_ESC, KEY_LEFT_ALT, NO_KEY}
};

static const keyboard_joystick_layout default_joystick = {
  KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW,
  NO_KEY, NO_KEY
};

static const int alternative_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_ALT, '5', '1', 'e', KEY_TAB, NO_KEY},
  {NO_KEY, ' ', '6', '2', NO_KEY, 'j', NO_KEY},
  {NO_KEY, KEY_LEFT_CTRL, '7', '3', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 'z', '8', '4', 'r', 'q', NO_KEY}
};

static const keyboard_joystick_layout alternative_joystick = {
  'w', 's', 'a', 'd', KEY_LEFT_SHIFT, NO_KEY};

static const int cyberpunk_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_SHIFT, 'q', '1', 'f', KEY_ESC, NO_KEY},
  {NO_KEY, ' ', 'e', '2', NO_KEY, 'j', NO_KEY},
  {NO_KEY, 'c', 'r', '3', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, KEY_TAB, 'z', '4', 'b', 'x', NO_KEY}
};

static const keyboard_joystick_layout cyberpunk_joystick = {
  'w', 's', 'a', 'd', NO_KEY, NO_KEY
};

static const int prey_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_SHIFT, 'q', 'e', 'f', KEY_ESC, NO_KEY},
  {NO_KEY, KEY_LEFT_CTRL, 'v', 'r', NO_KEY, 'i', NO_KEY},
  {NO_KEY, ' ', 'l', '1', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 'z', 'x', '2', 'g', KEY_TAB, NO_KEY}
};

static const keyboard_joystick_layout prey_joystick = {
  'w', 's', 'a', 'd', NO_KEY, NO_KEY
};

static const int starsector_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, 'r', '5', '1', ' ', KEY_ESC, NO_KEY},
  {NO_KEY, 'f', '6', '2', NO_KEY, 'c', NO_KEY},
  {NO_KEY, 't', '7', '3', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 'v', '8', '4', KEY_LEFT_SHIFT, KEY_LEFT_CTRL, NO_KEY}
};

static const keyboard_joystick_layout starsector_joystick = {
  'w', 's', 'a', 'd', NO_KEY, NO_KEY
};

static keypad_layout current_layout = DEFAULT_KEYBOARD_LAYOUT;

static const keyboard_set key_sets[MAX_KEYBOARD_LAYOUT] = {
  {DEFAULT_KEYBOARD_LAYOUT, &default_keypad, &default_joystick},
  {ALTERNATIVE_KEYBOARD_LAYOUT, &alternative_keypad, &alternative_joystick},
  {CYBERPUNK_KEYBOARD_LAYOUT, &cyberpunk_keypad, &cyberpunk_joystick},
  {PREY_KEYBOARD_LAYOUT, &prey_keypad, &prey_joystick},
  {STARSECTOR_KEYBOARD_LAYOUT, &starsector_keypad, &starsector_joystick}
};

static void
update_layout(keypad_layout layout) {
  for (uint8_t it = DEFAULT_KEYBOARD_LAYOUT; it < MAX_KEYBOARD_LAYOUT; it++)
    if (key_sets[it].id == layout) {
      for (uint8_t row = 0; row < ROW_NB; row++)
        for (uint8_t col = 0; col < COLUMN_NB; col++) {
          const keypad_array * ar = key_sets[it].keypad;
          keypad[row][col].value = (*ar)[row][col];
        }
      joystick.up_value = key_sets[it].joystick->up_value;
      joystick.down_value = key_sets[it].joystick->down_value;
      joystick.left_value = key_sets[it].joystick->left_value;
      joystick.right_value = key_sets[it].joystick->right_value;
      joystick.low_mod_value = key_sets[it].joystick->low_mod_value;
      joystick.high_mod_value = key_sets[it].joystick->high_mod_value;
    }
  current_layout = layout;
}

void
setup_keyboard_mode(void) {
  Serial.println("Starting keyboard mode");
  Keyboard.begin();
  update_layout(current_layout);
  setup_actions(&keyboard_press_key, &keyboard_release_key,
                &keyboard_joystick_update, &keyboard_joystick_update_both,
                &keyboard_mode_change);
}

void
end_keyboard_mode(void) {
  Keyboard.end();
  setup_actions(NULL, NULL, NULL, NULL, NULL);
}

static keypad_layout
cycle_keypad_layouts(void) {
  if ((current_layout + 1) < MAX_KEYBOARD_LAYOUT)
    return (keypad_layout)(current_layout + 1);
  else
    return DEFAULT_KEYBOARD_LAYOUT;
}

void
keyboard_mode_change(void) {
  Serial.println("Cycling keypad layouts");
  update_layout(cycle_keypad_layouts());
}

void
keyboard_press_key(key_item key) {
  Serial.print("[");
  Serial.print(key.code);
  Serial.println("] PRESSED");
  Keyboard.press(key.value);
}

void
keyboard_release_key(key_item key) {
  Serial.print("[");
  Serial.print(key.code);
  Serial.println("] RELEASED");
  Keyboard.release(key.value);
}

void
keyboard_press_joykey(int value) {
  if (value != NO_KEY)
    Keyboard.press(value);
}

void
keyboard_release_joykey(int value) {
  if (value != NO_KEY)
    Keyboard.release(value);
}

static inline bool
at_least_one_axis_high(void) {
  return (up_axis == JOY_STATE_HIGH || down_axis == JOY_STATE_HIGH ||
          left_axis == JOY_STATE_HIGH || right_axis == JOY_STATE_HIGH);
}

static inline bool
at_least_one_axis_low(void) {
  return (up_axis == JOY_STATE_LOW || down_axis == JOY_STATE_LOW ||
          left_axis == JOY_STATE_LOW || right_axis == JOY_STATE_LOW);
}

static inline bool
at_least_one_axis_not_high(void) {
  return (!(up_axis == JOY_STATE_HIGH && down_axis == JOY_STATE_HIGH &&
            left_axis == JOY_STATE_HIGH && right_axis == JOY_STATE_HIGH));
}

static inline bool
all_axis_down(void) {
  return (up_axis == JOY_STATE_OFF && down_axis == JOY_STATE_OFF &&
          left_axis == JOY_STATE_OFF && right_axis == JOY_STATE_OFF);
}

static inline bool
apply_low_mod(void) {
  return (at_least_one_axis_low() &&
          (up_axis <= JOY_STATE_LOW && down_axis <= JOY_STATE_LOW &&
           left_axis <= JOY_STATE_LOW && right_axis <= JOY_STATE_LOW));
}

static inline bool
apply_high_mod(void) {
  return (at_least_one_axis_high() &&
          (up_axis != JOY_STATE_LOW && down_axis != JOY_STATE_LOW &&
           left_axis != JOY_STATE_LOW && right_axis != JOY_STATE_LOW));
}

void
keyboard_joystick_update(joystick_axis axis, int value) {

  if (axis == JOY_AXIS_X) {
    if (value < JOY_VERYLOW) {
      left_changed = ((left_axis == JOY_STATE_OFF) ? true : false);
      left_axis = JOY_STATE_HIGH;
    } else if (value <= JOY_LOW) {
      left_changed = ((left_axis == JOY_STATE_OFF) ? true : false);
      left_axis = JOY_STATE_LOW;
    } else {
      left_changed = ((left_axis != JOY_STATE_OFF) ? true : false);
      left_axis = JOY_STATE_OFF;
    }
    if (value > JOY_VERYHIGH) {
      right_changed = ((right_axis == JOY_STATE_OFF) ? true : false);
      right_axis = JOY_STATE_HIGH;
    } else if (value >= JOY_HIGH) {
      right_changed = ((right_axis == JOY_STATE_OFF) ? true : false);
      right_axis = JOY_STATE_LOW;
    } else {
      right_changed = ((right_axis != JOY_STATE_OFF) ? true : false);
      right_axis = JOY_STATE_OFF;
    }
  } else if (axis == JOY_AXIS_Y) {
    if (value < JOY_VERYLOW) {
      up_changed = ((up_axis == JOY_STATE_OFF) ? true : false);
      up_axis = JOY_STATE_HIGH;
    } else if (value <= JOY_LOW) {
      up_changed = ((up_axis == JOY_STATE_OFF) ? true : false);
      up_axis = JOY_STATE_LOW;
    } else {
      up_changed = ((up_axis != JOY_STATE_OFF) ? true : false);
      up_axis = JOY_STATE_OFF;
    }
    if (value > JOY_VERYHIGH) {
      down_changed = ((down_axis == JOY_STATE_OFF) ? true : false);
      down_axis = JOY_STATE_HIGH;
    } else if (value >= JOY_HIGH) {
      down_changed = ((down_axis == JOY_STATE_OFF) ? true : false);
      down_axis = JOY_STATE_LOW;
    } else {
      down_changed = ((down_axis != JOY_STATE_OFF) ? true : false);
      down_axis = JOY_STATE_OFF;
    }
  }

  if (low_mod_active == false && apply_low_mod() == true) {
    if (joystick.low_mod_value != NO_KEY)
      Keyboard.press(joystick.low_mod_value);
    Serial.println("Low modifier press");
    low_mod_active = true;
  } else if (low_mod_active == true &&
             (all_axis_down() == true || at_least_one_axis_high() == true)) {
    if (joystick.low_mod_value != NO_KEY)
      Keyboard.release(joystick.low_mod_value);
    Serial.println("Low modifier release");
    low_mod_active = false;
  }

  if (high_mod_active == false && apply_high_mod() == true) {
    if (joystick.low_mod_value != NO_KEY)
      Keyboard.press(joystick.high_mod_value);
    Serial.println("High modifier press");
    high_mod_active = true;
  } else if (high_mod_active == true && at_least_one_axis_low()) {
    if (joystick.low_mod_value != NO_KEY)
      Keyboard.release(joystick.high_mod_value);
    Serial.println("High modifier release");
    high_mod_active = false;
  }

  if (left_changed == true) {
    if (left_axis >= JOY_STATE_LOW) {
      Keyboard.press(joystick.left_value);
      Serial.println("Left press");
    } else {
      Keyboard.release(joystick.left_value);
      Serial.println("Left release");
    }
  }

  if (right_changed == true) {
    if (right_axis >= JOY_STATE_LOW) {
      Keyboard.press(joystick.right_value);
      Serial.println("Right press");
    } else {
      Keyboard.release(joystick.right_value);
      Serial.println("Right release");
    }
  }

  if (up_changed == true) {
    if (up_axis >= JOY_STATE_LOW) {
      Keyboard.press(joystick.up_value);
      Serial.println("Up press");
    } else {
      Keyboard.release(joystick.up_value);
      Serial.println("Up release");
    }
  }

  if (down_changed == true) {
    if (down_axis >= JOY_STATE_LOW) {
      Keyboard.press(joystick.down_value);
      Serial.println("Down press");
    } else {
      Keyboard.release(joystick.down_value);
      Serial.println("Down release");
    }
  }

}

void
keyboard_joystick_update_both(int x_value, int y_value) {
  keyboard_joystick_update(JOY_AXIS_X, x_value);
  keyboard_joystick_update(JOY_AXIS_Y, y_value);
}

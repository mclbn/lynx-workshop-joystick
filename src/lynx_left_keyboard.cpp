#include "lynx_left_keyboard.h"

static const int default_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_F9, KEY_F5, KEY_F1, KEY_RETURN, KEY_TAB, NO_KEY},
  {NO_KEY, KEY_F10, KEY_F6, KEY_F2, NO_KEY, KEY_LEFT_GUI, NO_KEY},
  {NO_KEY, KEY_F11, KEY_F7, KEY_F3, NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, KEY_F12, KEY_F8, KEY_F4, KEY_ESC, KEY_LEFT_ALT, NO_KEY}
};

static const joystick_layout default_joystick = {
  KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW,
  NO_KEY, NO_KEY
};

static const int alternative_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_ALT, '5', '1', 'e', KEY_TAB, NO_KEY},
  {NO_KEY, ' ', '6', '2', NO_KEY, 'j', NO_KEY},
  {NO_KEY, KEY_LEFT_CTRL, '7', '3', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 'z', '8', '4', 'r', 'q', NO_KEY}
};

static const joystick_layout alternative_joystick = {
  'w', 's', 'a', 'd', KEY_LEFT_SHIFT, NO_KEY};

static const int cyberpunk_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_SHIFT, 'q', '1', 'f', KEY_ESC, NO_KEY},
  {NO_KEY, ' ', 'e', '2', NO_KEY, 'j', NO_KEY},
  {NO_KEY, 'c', 'r', '3', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, KEY_TAB, 'z', '4', 'b', 'x', NO_KEY}
};

static const joystick_layout cyberpunk_joystick = {
  'w', 's', 'a', 'd', NO_KEY, NO_KEY
};

static const int prey_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_SHIFT, 'q', 'e', 'f', KEY_ESC, NO_KEY},
  {NO_KEY, KEY_LEFT_CTRL, 'v', 'r', NO_KEY, 'i', NO_KEY},
  {NO_KEY, ' ', 'l', '1', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 'z', 'x', '2', 'g', KEY_TAB, NO_KEY}
};

static const joystick_layout prey_joystick = {
  'w', 's', 'a', 'd', NO_KEY, NO_KEY
};

static const int starsector_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, KEY_LEFT_SHIFT, '5', '1', 'v', 'f', NO_KEY},
  {NO_KEY, KEY_LEFT_CTRL, '6', '2', NO_KEY, ' ', NO_KEY},
  {NO_KEY, KEY_RETURN, '7', '3', NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, KEY_TAB, '8', '4', '9', '0', NO_KEY}
};

static const joystick_layout starsector_joystick = {
  'w', 's', 'a', 'd', NO_KEY, NO_KEY
};

static keypad_layout current_layout = DEFAULT_LAYOUT;

static const keyboard_set key_sets[MAX_LAYOUT] = {
  {DEFAULT_LAYOUT, &default_keypad, &default_joystick},
  {ALTERNATIVE_LAYOUT, &alternative_keypad, &alternative_joystick},
  {CYBERPUNK_LAYOUT, &cyberpunk_keypad, &cyberpunk_joystick},
  {PREY_LAYOUT, &prey_keypad, &prey_joystick},
  {STARSECTOR_LAYOUT, &starsector_keypad, &starsector_joystick}
};

static void
update_layout(keypad_layout layout) {
  for (uint8_t it = DEFAULT_LAYOUT; it < MAX_LAYOUT; it++)
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
                &keyboard_press_joykey, &keyboard_release_joykey,
                &keyboard_mode_change);
}

void
end_keyboard_mode(void) {
  Keyboard.end();
  setup_actions(NULL, NULL, NULL, NULL, NULL);
}

static keypad_layout
cycle_keypad_layouts(void) {
  if ((current_layout + 1) < MAX_LAYOUT)
    return (keypad_layout)(current_layout + 1);
  else
    return DEFAULT_LAYOUT;
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

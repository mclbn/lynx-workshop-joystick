#include "lynx_left_activity.h"

input_mode current_mode = KEYBOARD_MODE;

unsigned long time_counter = 0;
bool activity = false;

int x_axis = 0;
int y_axis = 0;

bool up_changed = false;
bool down_changed = false;
bool left_changed = false;
bool right_changed = false;
bool low_mod_active = false;
bool high_mod_active = false;

joy_state up_axis = JOY_STATE_OFF;
joy_state down_axis = JOY_STATE_OFF;
joy_state left_axis = JOY_STATE_OFF;
joy_state right_axis = JOY_STATE_OFF;

button_layout keypad[ROW_NB][COLUMN_NB] = {
  {
    {NO_CODE, "NULL", NO_KEY, false}, // Unused
    {9, "r1", NO_KEY, false},
    {5, "m1", NO_KEY, false},
    {1, "i1", NO_KEY, false},
    {17, "t1", NO_KEY, false},
    {16, "t2", NO_KEY, false},
    {NO_CODE, "NULL", NO_KEY, false} // Unused
  },
  {{13, "p1", NO_KEY, false}, // Keyboard mode change
   {10, "r2", NO_KEY, false},
   {6, "m2", NO_KEY, false},
   {2, "i2", NO_KEY, false},
   {20, "NULL", NO_KEY, false}, // Unused
   {19, "t3", NO_KEY, false},
   {18, "NULL", NO_KEY, false}},
  {{14, "p2", NO_KEY, false}, // Joystick mode change
   {11, "r3", NO_KEY, false},
   {7, "m3", NO_KEY, false},
   {3, "i3", NO_KEY, false},
   {23, "NULL", NO_KEY, false},
   {22, "NULL", NO_KEY, false},
   {21, "NULL", NO_KEY, false}},
  {
    {15, "p3", NO_KEY, false}, // Mouse mode change
    {12, "r4", NO_KEY, false},
    {8, "m4", NO_KEY, false},
    {4, "i4", NO_KEY, false},
    {25, "t4", NO_KEY, false},
    {24, "t5", NO_KEY, false},
    {NO_CODE, "NULL", NO_KEY, false} // Unused
  }};

joystick_layout joystick = {NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY};

key_item key_list[MAX_KEYS];

activity_actions actions = {
  NULL, NULL, NULL, NULL
};

void
setup_activity(void) {
  time_counter = 0;
  memset(key_list, 0, sizeof(key_list));
}

void
setup_actions(void (*press_button)(key_item),
              void (*release_button)(key_item),
              void (*press_joystick)(int),
              void (*release_joystick)(int),
              void (*mode_change)(void)) {
  actions.press_button = press_button;
  actions.release_button = release_button;
  actions.press_joystick = press_joystick;
  actions.release_joystick = release_joystick;
  actions.mode_change = mode_change;
}

static int8_t
get_item_by_value(char value) {
  for (uint8_t i = 0; i < MAX_KEYS; i++)
    if (key_list[i].value == value)
      return i;
  return -1;
}

static int8_t
get_item_by_code(int8_t code) {
  for (uint8_t i = 0; i < MAX_KEYS; i++)
    if (key_list[i].code == code)
      return i;
  return -1;
}

static void
set_state(uint8_t idx, key_state state) {
  key_list[idx].state = state;
  key_list[idx].state_changed = true;
}

static void
update_state(uint8_t idx, bool active) {
  key_list[idx].state_changed = false;

  switch (key_list[idx].state) {
  case IDLE:
    if (active == true) {
      set_state(idx, PRESSED);
      key_list[idx].hold_timer = millis();
    }
    break;
  case PRESSED:
    if ((millis() - key_list[idx].hold_timer) > HOLD_TIME)
      set_state(idx, HELD);
    else if (active == false)
      set_state(idx, RELEASED);
    break;
  case HELD: // legacy from the Keypad library, but not used in this program
    if (active == false)
      set_state(idx, RELEASED);
    break;
  case RELEASED:
    set_state(idx, IDLE);
    break;
  default:
    break;
  }
}

static bool
update_buffer(void) {
  bool activity = false;

  for (uint8_t i = 0; i < MAX_KEYS; i++) {
    if (key_list[i].state == IDLE) {
      key_list[i].value = NO_KEY;
      key_list[i].code = NO_CODE;
      key_list[i].state_changed = false;
    }
  }

  for (uint8_t row = 0; row < ROW_NB; row++) {
    for (uint8_t col = 0; col < COLUMN_NB; col++) {
      // do stuff
      bool active = keypad[row][col].active;
      char value = keypad[row][col].value;
      int8_t code = keypad[row][col].code;
      int8_t idx = get_item_by_code(code);

      if (idx > -1 && code != NO_CODE) { // already in list, updating
        update_state(idx, active);
      } else if (active) {
        for (uint8_t i = 0; i < MAX_KEYS; i++) {
          if (key_list[i].value == NO_KEY) { // looking for a free slot
            key_list[i].value = value;
            key_list[i].code = code;
            key_list[i].state = IDLE; // IDLE value for new keys in the buffer
            update_state(i, active);
            break;
          }
        }
      }
    }
  }

  for (uint8_t i = 0; i < MAX_KEYS; i++) {
    if (key_list[i].state_changed) {
      activity = true;
      break;
    }
  }

  return activity;
}

static bool
get_keys(void) {
  bool activity = false;

  if ((millis() - time_counter) > DEBOUNCE_DELAY) {
    for (uint8_t output_pin = 0; output_pin < ROW_NB; output_pin++) {
      digitalWrite(output_pins[output_pin], LOW);
      for (uint8_t input_pin = 0; input_pin < COLUMN_NB; input_pin++) {
        if (digitalRead(input_pins[input_pin]) == LOW) // key pressed
          keypad[output_pin][input_pin].active = true;
        else // key not pressed
          keypad[output_pin][input_pin].active = false;
      }
      digitalWrite(output_pins[output_pin], HIGH);
    }
    activity = update_buffer();
    time_counter = millis();
  }
  return activity;
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

static void
switch_mode(input_mode new_mode) {
  switch (current_mode) {
  case (KEYBOARD_MODE):
    end_keyboard_mode();
    break;
  case (JOYSTICK_MODE):
    break;
  case (MOUSE_MODE):
    break;
  default:
    break;
  }

  switch (new_mode) {
  case (KEYBOARD_MODE):
    setup_keyboard_mode();
    break;
  case (JOYSTICK_MODE):
    break;
  case (MOUSE_MODE):
    break;
  default:
    break;
  }
  current_mode = new_mode;
}

static void
handle_mode_change(int key_code) {
  switch (key_code) {
  case 13: { // Keyboard mode
    switch (current_mode) {
    case KEYBOARD_MODE: {
      actions.mode_change();
      break;
    }
    case JOYSTICK_MODE: {
      end_joystick_mode();
      setup_keyboard_mode();
      current_mode = KEYBOARD_MODE;
      break;
    }
    case MOUSE_MODE: {
      end_mouse_mode();
      setup_keyboard_mode();
      current_mode = KEYBOARD_MODE;
      break;
    }
    default:
      break;
    }
    break;
  }
  case 14: { // Joystick mode
    switch (current_mode) {
    case KEYBOARD_MODE: {
      end_keyboard_mode();
      setup_joystick_mode();
      current_mode = JOYSTICK_MODE;
      break;
    }
    case JOYSTICK_MODE: {
      actions.mode_change();
      break;
    }
    case MOUSE_MODE: {
      end_mouse_mode();
      setup_joystick_mode();
      current_mode = JOYSTICK_MODE;
      break;
    }
    default:
      break;
    }
    break;
  }
  case 15: { // Mouse mode
    switch (current_mode) {
    case KEYBOARD_MODE: {
      end_keyboard_mode();
      setup_mouse_mode();
      current_mode = MOUSE_MODE;
      break;
    }
    case JOYSTICK_MODE: {
      end_joystick_mode();
      setup_mouse_mode();
      current_mode = MOUSE_MODE;
      break;
    }
    case MOUSE_MODE: {
      actions.mode_change();
      break;
    }
    default:
      break;
    }
    break;
  }
  default:
    break;
  }
}

void
handle_activity(void) {
  bool activity = get_keys();

  if (activity) {
    for (uint8_t i = 0; i < MAX_KEYS; i++) {
      if (key_list[i].state == PRESSED || key_list[i].state == HELD) {
        if (key_list[i].code == 13 // Keyboard mode
            || key_list[i].code == 14 // Joystick mode
            || key_list[i].code == 15) // Mouse mode
          continue;
        else
          actions.press_button(key_list[i]);
      } else if (key_list[i].state == RELEASED) {
        if (key_list[i].code == 13 // Keyboard mode
            || key_list[i].code == 14 // Joystick mode
            || key_list[i].code == 15) // Mouse mode
          handle_mode_change(key_list[i].code);
        else
          actions.release_button(key_list[i]);
      }
    }
  }

  x_axis = analogRead(VRX_PIN);
  y_axis = analogRead(VRY_PIN);

  if (x_axis < JOY_VERYLOW) {
    left_changed = ((left_axis == JOY_STATE_OFF) ? true : false);
    left_axis = JOY_STATE_HIGH;
  } else if (x_axis <= JOY_LOW) {
    left_changed = ((left_axis == JOY_STATE_OFF) ? true : false);
    left_axis = JOY_STATE_LOW;
  } else {
    left_changed = ((left_axis != JOY_STATE_OFF) ? true : false);
    left_axis = JOY_STATE_OFF;
  }

  if (x_axis > JOY_VERYHIGH) {
    right_changed = ((right_axis == JOY_STATE_OFF) ? true : false);
    right_axis = JOY_STATE_HIGH;
  } else if (x_axis >= JOY_HIGH) {
    right_changed = ((right_axis == JOY_STATE_OFF) ? true : false);
    right_axis = JOY_STATE_LOW;
  } else {
    right_changed = ((right_axis != JOY_STATE_OFF) ? true : false);
    right_axis = JOY_STATE_OFF;
  }

  if (y_axis < JOY_VERYLOW) {
    up_changed = ((up_axis == JOY_STATE_OFF) ? true : false);
    up_axis = JOY_STATE_HIGH;
  } else if (y_axis <= JOY_LOW) {
    up_changed = ((up_axis == JOY_STATE_OFF) ? true : false);
    up_axis = JOY_STATE_LOW;
  } else {
    up_changed = ((up_axis != JOY_STATE_OFF) ? true : false);
    up_axis = JOY_STATE_OFF;
  }

  if (y_axis > JOY_VERYHIGH) {
    down_changed = ((down_axis == JOY_STATE_OFF) ? true : false);
    down_axis = JOY_STATE_HIGH;
  } else if (y_axis >= JOY_HIGH) {
    down_changed = ((down_axis == JOY_STATE_OFF) ? true : false);
    down_axis = JOY_STATE_LOW;
  } else {
    down_changed = ((down_axis != JOY_STATE_OFF) ? true : false);
    down_axis = JOY_STATE_OFF;
  }

  if (low_mod_active == false && apply_low_mod() == true) {
    actions.press_joystick(joystick.low_mod_value);
    Serial.println("Low modifier press");
    low_mod_active = true;
  } else if (low_mod_active == true &&
             (all_axis_down() == true || at_least_one_axis_high() == true)) {
    actions.release_joystick(joystick.low_mod_value);
    Serial.println("Low modifier release");
    low_mod_active = false;
  }

  if (high_mod_active == false && apply_high_mod() == true) {
    actions.press_joystick(joystick.high_mod_value);
    Serial.println("High modifier press");
    high_mod_active = true;
  } else if (high_mod_active == true && at_least_one_axis_low()) {
    actions.release_joystick(joystick.high_mod_value);
    Serial.println("High modifier release");
    high_mod_active = false;
  }

  if (left_changed == true) {
    if (left_axis >= JOY_STATE_LOW) {
      actions.press_joystick(joystick.left_value);
      Serial.println("Left press");
    } else {
      actions.release_joystick(joystick.left_value);
      Serial.println("Left release");
    }
  }

  if (right_changed == true) {
    if (right_axis >= JOY_STATE_LOW) {
      actions.press_joystick(joystick.right_value);
      Serial.println("Right press");
    } else {
      actions.release_joystick(joystick.right_value);
      Serial.println("Right release");
    }
  }

  if (up_changed == true) {
    if (up_axis >= JOY_STATE_LOW) {
      actions.press_joystick(joystick.up_value);
      Serial.println("Up press");
    } else {
      actions.release_joystick(joystick.up_value);
      Serial.println("Up release");
    }
  }

  if (down_changed == true) {
    if (down_axis >= JOY_STATE_LOW) {
      actions.press_joystick(joystick.down_value);
      Serial.println("Down press");
    } else {
      actions.release_joystick(joystick.down_value);
      Serial.println("Down release");
    }
  }
}

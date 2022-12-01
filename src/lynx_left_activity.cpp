#include "lynx_left_activity.h"

static input_mode current_mode = KEYBOARD_MODE;

static unsigned long debounce_counter = 0;
static unsigned long response_counter = 0;

static int last_x_axis = -1;
static int last_y_axis = -1;

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

static key_item key_list[MAX_KEYS];

static activity_actions actions = {
  NULL, NULL, NULL, NULL
};

void
setup_activity(void) {
  debounce_counter = 0;
  response_counter = 0;
  memset(key_list, 0, sizeof(key_list));
}

void
setup_actions(void (*press_button)(key_item),
              void (*release_button)(key_item),
              void (*joystick_update)(joystick_axis, int),
              void (*joystick_update_both)(int, int),
              void (*mode_change)(void)) {
  actions.press_button = press_button;
  actions.release_button = release_button;
  actions.joystick_update = joystick_update;
  actions.joystick_update_both = joystick_update_both;
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

  if ((millis() - debounce_counter) > DEBOUNCE_DELAY) {
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
    debounce_counter = millis();
  }
  return activity;
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
  bool key_activity = get_keys();

  if (key_activity) {
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

  if ((millis() - response_counter) > RESPONSE_DELAY) {
    int x_axis = analogRead(VRX_PIN);
    int y_axis = analogRead(VRY_PIN);

    if (current_mode == MOUSE_MODE
        || ((x_axis != last_x_axis) && (y_axis != last_y_axis))) {
      actions.joystick_update_both(x_axis, y_axis);
      last_x_axis = x_axis;
      last_y_axis = y_axis;
    } else {
      if (x_axis != last_x_axis) {
        actions.joystick_update(JOY_AXIS_X, x_axis);
        last_x_axis = x_axis;
      }

      if (y_axis != last_y_axis) {
        actions.joystick_update(JOY_AXIS_Y, y_axis);
        last_y_axis = y_axis;
      }
    }
    response_counter = millis();
  }
}

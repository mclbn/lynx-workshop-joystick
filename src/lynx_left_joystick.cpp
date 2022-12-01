#include "lynx_left_joystick.h"

static const int default_button_map[ROW_NB][COLUMN_NB] = {
  {NO_KEY, 13, 9, 5, 0, 1, NO_KEY},
  {NO_KEY, 14, 10, 6, NO_KEY, 4, NO_KEY},
  {NO_KEY, 15, 11, 7, NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 16, 12, 8, 2, 3, NO_KEY}
};

// Create the Joystick
static Joystick_ joystick_controller(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                          JOYSTICK_BUTTON_NUMBER, 0, // Button Count, Hat Switch Count
                          true, true, false,     // X and Y, but no Z Axis
                          false, false, false,   // No Rx, Ry, or Rz
                          false, false,          // No rudder or throttle
                          false, false, false);  // No accelerator, brake, or steering

int last_buttons_state[JOYSTICK_BUTTON_NUMBER];

static const joystick_set joy_sets[MAX_JOYSTICK_LAYOUT] = {
  {DEFAULT_JOYSTICK_LAYOUT, &default_button_map}
};

static joystick_layout current_layout = DEFAULT_JOYSTICK_LAYOUT;

static void
update_layout(joystick_layout layout) {
  for (uint8_t it = DEFAULT_JOYSTICK_LAYOUT; it < MAX_JOYSTICK_LAYOUT; it++)
    if (joy_sets[it].id == layout) {
      for (uint8_t row = 0; row < ROW_NB; row++)
        for (uint8_t col = 0; col < COLUMN_NB; col++) {
          const keypad_array * ar = joy_sets[it].keypad;
          keypad[row][col].value = (*ar)[row][col];
        }
    }
  current_layout = layout;
}

void
setup_joystick_mode(void) {
  Serial.println("Starting joystick mode");
  joystick_controller.begin();
  memset(last_buttons_state, 0, sizeof(last_buttons_state));
  joystick_controller.setXAxisRange(0, 1023);
  joystick_controller.setYAxisRange(0, 1023);
  update_layout(current_layout);
  setup_actions(&joystick_press_key, &joystick_release_key,
                &joystick_joystick_update, &joystick_joystick_update_both,
                &joystick_mode_change);
}

void
end_joystick_mode(void) {
  joystick_controller.end();
  setup_actions(NULL, NULL, NULL, NULL, NULL);
}

static joystick_layout
cycle_joystick_layouts(void) {
  if ((current_layout + 1) < MAX_JOYSTICK_LAYOUT)
    return (joystick_layout)(current_layout + 1);
  else
    return DEFAULT_JOYSTICK_LAYOUT;
}

void
joystick_mode_change(void) {
  Serial.println("Cycling joystick layouts");
  update_layout(cycle_joystick_layouts());
}

void
joystick_press_key(key_item key) {
  Serial.print("[");
  Serial.print(key.value);
  Serial.println("] PRESSED");
  joystick_controller.pressButton(key.value);
}

void joystick_release_key(key_item key) {
  Serial.print("[");
  Serial.print(key.value);
  Serial.println("] RELEASED");
  joystick_controller.releaseButton(key.value);
}

void
joystick_joystick_update(joystick_axis axis, int value) {
  if (axis == JOY_AXIS_X) {
    Serial.print("X: ");
    Serial.print(value);
    Serial.println("");
    joystick_controller.setXAxis(value);
  } else if (axis == JOY_AXIS_Y) {
    Serial.print("Y: ");
    Serial.print(value);
    Serial.println("");
    joystick_controller.setYAxis(value);
  }
  joystick_controller.sendState();
}

void joystick_joystick_update_both(int x_value, int y_value) {
  joystick_joystick_update(JOY_AXIS_X, x_value);
  joystick_joystick_update(JOY_AXIS_Y, y_value);
}

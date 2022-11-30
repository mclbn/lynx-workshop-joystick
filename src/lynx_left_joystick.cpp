#include "lynx_left_joystick.h"

static const int default_keypad[ROW_NB][COLUMN_NB] = {
  {NO_KEY, 13, 9, 5, 0, 1, NO_KEY},
  {NO_KEY, 14, 10, 6, NO_KEY, 4, NO_KEY},
  {NO_KEY, 15, 11, 7, NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, 16, 12, 8, 2, 3, NO_KEY}};

static const joystick_layout default_joystick = {KEY_UP_ARROW,   KEY_DOWN_ARROW,
                                    KEY_LEFT_ARROW, KEY_RIGHT_ARROW,
                                    NO_KEY,         NO_KEY};

// Create the Joystick
static Joystick_ joystick_controller(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                          JOYSTICK_BUTTON_NUMBER, 0, // Button Count, Hat Switch Count
                          true, true, false,     // X and Y, but no Z Axis
                          false, false, false,   // No Rx, Ry, or Rz
                          false, false,          // No rudder or throttle
                          false, false, false);  // No accelerator, brake, or steering

int last_buttons_state[JOYSTICK_BUTTON_NUMBER];

static keypad_layout current_layout = DEFAULT_LAYOUT;

static void
update_layout(keypad_layout layout) {
  char value = NO_KEY;

  for (uint8_t row = 0; row < ROW_NB; row++)
    for (uint8_t col = 0; col < COLUMN_NB; col++) {
      switch (layout) {
      default:
        value = default_keypad[row][col];
        break;
      }
      keypad[row][col].value = value;
    }
}

void
setup_joystick_mode(void) {
  Serial.println("Starting joystick mode");
  joystick_controller.begin();
  joystick_controller.setXAxisRange(-1, 1);
  joystick_controller.setYAxisRange(-1, 1);
  memset(last_buttons_state, 0, sizeof(last_buttons_state));
  update_layout(current_layout);
  setup_actions(&joystick_press_key, &joystick_release_key,
                &joystick_press_joykey, &joystick_release_joykey,
                &joystick_mode_change);
}

void
end_joystick_mode(void) {
  joystick_controller.end();
  setup_actions(NULL, NULL, NULL, NULL, NULL);
}

void
joystick_mode_change(void) {

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

void joystick_press_joykey(int value) {

}

void joystick_release_joykey(int value) {

}

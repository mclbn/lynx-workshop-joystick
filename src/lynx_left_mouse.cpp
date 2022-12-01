#include "lynx_left_mouse.h"

void setup_mouse_mode(void) {
  Serial.println("Starting mouse mode");
  Mouse.begin();
  setup_actions(&mouse_press_key, &mouse_release_key,
                &mouse_joystick_update, &mouse_mode_change);
}

void end_mouse_mode(void) {
  Mouse.end();
  setup_actions(NULL, NULL, NULL, NULL);
}

void mouse_mode_change(void) {

}

void mouse_press_key(key_item key) {

}

void mouse_release_key(key_item key) {

}

void
mouse_joystick_update(joystick_axis axis, int value) {

}

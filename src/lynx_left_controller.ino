#include "lynx_left_controller.h"

void
setup(void) {
  pinMode(SAFETY_PIN, INPUT);
  digitalWrite(SAFETY_PIN, HIGH);

  while (digitalRead(SAFETY_PIN)) {
    //safety loop: as long as safety pin is disconnected
  }

  for (uint8_t i = 0; i < ROW_NB; i++) { // outputs
    pinMode(output_pins[i], OUTPUT);
    digitalWrite(output_pins[i], HIGH);
  }

  for(uint8_t i = 0; i < COLUMN_NB; i++) // inputs
    pinMode(input_pins[i],INPUT_PULLUP);

  Serial.begin(9600);
  setup_activity();
  setup_keyboard_mode(); //  Default is keyboard
}

void
loop(void) {
  handle_activity();
}

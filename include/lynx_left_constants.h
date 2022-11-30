#ifndef LYNX_LEFT_CONSTANTS_H
#define LYNX_LEFT_CONSTANTS_H

#define ROW_NB (4)
#define COLUMN_NB (7)

#define SAFETY_PIN 21 // connect pin 21 to GND
#define VRX_PIN A1                      //joystick set up
#define VRY_PIN A0

extern int input_pins[COLUMN_NB];
extern int output_pins[ROW_NB];

#endif // LYNX_LEFT_CONSTANTS_H

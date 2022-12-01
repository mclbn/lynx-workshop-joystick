#include "lynx_left_mouse.h"

static int last_x_position = 0;
static int last_y_position = 0;

static const int default_mouse_map[ROW_NB][COLUMN_NB] = {
  {NO_KEY, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_LEFT , NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY},
  {NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY, NO_KEY}
};

static const mouse_set mouse_sets[MAX_MOUSE_LAYOUT] = {
  {DEFAULT_MOUSE_LAYOUT, &default_mouse_map}
};

static mouse_layout current_layout = DEFAULT_MOUSE_LAYOUT;

static void
update_layout(mouse_layout layout) {
  for (uint8_t it = DEFAULT_MOUSE_LAYOUT; it < MAX_MOUSE_LAYOUT; it++)
    if (mouse_sets[it].id == layout) {
      for (uint8_t row = 0; row < ROW_NB; row++)
        for (uint8_t col = 0; col < COLUMN_NB; col++) {
          const keypad_array * ar = mouse_sets[it].keypad;
          keypad[row][col].value = (*ar)[row][col];
        }
    }
  current_layout = layout;
}

void setup_mouse_mode(void) {
  Serial.println("Starting mouse mode");
  Mouse.begin();
  update_layout(current_layout);
  setup_actions(&mouse_press_key, &mouse_release_key,
                &mouse_joystick_update, &mouse_joystick_update_both,
                &mouse_mode_change);
}

void end_mouse_mode(void) {
  Mouse.end();
  setup_actions(NULL, NULL, NULL, NULL, NULL);
}

static mouse_layout
cycle_mouse_layouts(void) {
  if ((current_layout + 1) < MAX_MOUSE_LAYOUT)
    return (mouse_layout)(current_layout + 1);
  else
    return DEFAULT_MOUSE_LAYOUT;
}

void mouse_mode_change(void) {
  Serial.println("Cycling mouse layouts");
  update_layout(cycle_mouse_layouts());
}

void mouse_press_key(key_item key) {
  if (key.value != NO_KEY)
    Mouse.press(key.value);
}

void mouse_release_key(key_item key) {
  if (key.value != NO_KEY)
    Mouse.release(key.value);
}

static int
get_mouse_distance(int value) {
  int reading = map(value, 0, 1023, 0, MOUSE_AXIS_RANGE);
  int distance = reading - MOUSE_AXIS_CENTER;

  if (abs(distance) < MOUSE_AXIS_THRESHOLD)
    distance = 0;

  return distance;
}

void
mouse_joystick_update(joystick_axis axis, int value) {
  Serial.println("Moving mouse axis");
  if (axis == JOY_AXIS_X)
    Mouse.move(get_mouse_distance(value), last_y_position);
  else if (axis == JOY_AXIS_Y)
    Mouse.move(last_x_position, get_mouse_distance(value));
}

void
mouse_joystick_update_both(int x_value, int y_value) {
  Serial.println("Moving both mouse axis");
  Mouse.move(get_mouse_distance(x_value), get_mouse_distance(y_value));
}

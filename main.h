#ifndef MAIN_h
#define MAIN_h

void passthrough_mode();
void translation_mode();
void translate_key_press(uint8_t adb_code, bool is_pressed);
bool is_button_changed_state();

#endif
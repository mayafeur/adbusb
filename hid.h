#ifndef KEYBOARD_h
#define KEYBOARD_h

#include <HID.h>

typedef struct {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keys[6];
} key_report_t;

void hid_init();
void hid_send_report();
void hid_press_key(uint8_t hid_code);
void hid_release_key(uint8_t hid_code);
void translate_key_press(uint8_t adb_code, bool is_pressed);

#endif

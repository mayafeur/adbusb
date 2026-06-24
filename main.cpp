#include <Arduino.h>
#include "gpio.h"
#include "main.h"
#include "adb.h"
#include "map.h"
#include "hid.h"

void passthrough_mode() {
    uint8_t command_byte;
    int i;
    while(true) {
        TCNT1 = 0;
        while(is_high(HOST_DATA) && TCNT1 <= 75 * 2);
        bool is_listen = (command_byte & 0b00001100) == 0b00001000;
        bool command_complete = !is_listen || i < -2;
        if(TCNT1 >= 75 * 2 && command_complete) {
            int kb_high_timeout = 185;  // max stop-to-start 260 - 75
            while(true) {
                TCNT1 = 0;
                while(is_high(KB_DATA) && TCNT1 < kb_high_timeout * 2);
                if(TCNT1 >= kb_high_timeout * 2) {
                    break;
                }
                kb_high_timeout = 75;
                set_low(HOST_DATA);
                while(!is_high(KB_DATA));
                set_release(HOST_DATA);
            }
        }
        while(is_high(HOST_DATA));
        set_low(KB_DATA);
        TCNT1 = 0;
        while(!is_high(HOST_DATA) && TCNT1 < 20000);  // timeout for button
        set_release(KB_DATA);
        int low_time = TCNT1 / 2;
        if(low_time > 700) {  // attention low
            i = 7;  // 8 bits command
            command_byte = 0;
        }
        else {
            bool bit = low_time < 50 ? 1 : 0;
            if(i >= 0) {  // if command_byte not complete 
                command_byte |= (bit << i);
            }
            i--;
            if(i == -2 && !is_high(KB_DATA)) {  // if first stop bit + keyboard SRQ
                set_low(HOST_DATA);
                while(!is_high(KB_DATA));
                set_release(HOST_DATA);
            }
        }
        if(!is_high(KB_SOFTPOWER)) {
            press_host_softpower();
        }
        if(is_button_changed_state()) {
            break;
        }
    }
}

void translation_mode() {
	while(true) {
		key_event_t ev = transaction_get_keys();
		translate_key_press(ev.keycode_1, ev.pressed_1);
		translate_key_press(ev.keycode_2, ev.pressed_2);
        delay(11);  // like an original adb host
        if(is_button_changed_state()) {
            break;
        }
	}
    transaction_flush_mouse();
}

void translate_key_press(uint8_t adb_code, bool is_pressed) {
	int usbcode = aek_to_hid(adb_code);
	if(usbcode) {
		if(usbcode == 0x39) {
			hid_press_key(usbcode);
			delay(100);
			hid_release_key(usbcode);
		}
		else if(is_pressed) {
			hid_press_key(usbcode);
		}
		else {
			hid_release_key(usbcode);
		}
	}
}

bool is_button_changed_state() {
    if(!is_high(SWITCH_BUTTON)) {
        transaction_set_leds(true, true, true);
        delay(200);
        transaction_set_leds(false, false, false);
        return true;
    }
    return false;
}
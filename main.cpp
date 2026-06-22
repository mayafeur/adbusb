#include <Arduino.h>
#include "conf.h"
#include "main.h"
#include "adb.h"
#include "map.h"
#include "hid.h"

bool is_button_activated() {
	return !(SWITCH_PIN_REGISTER & (1 << SWITCH_PIN_NUMBER));
}

void passthrough_mode() {
    uint8_t command_byte;
    int i;
    while(is_button_activated()) {
        TCNT1 = 0;
        while(is_host_high() && TCNT1 <= 75 * 2);
        bool is_listen = (command_byte & 0b00001100) == 0b00001000;
        bool command_complete = !is_listen || i < -2;
        if(TCNT1 >= 75 * 2 && command_complete) {
            int kb_high_timeout = 185;  // max stop-to-start 260 - 75
            while(true) {
                TCNT1 = 0;
                while(is_kb_high() && TCNT1 < kb_high_timeout * 2);
                if(TCNT1 >= kb_high_timeout * 2) {
                    break;
                }
                kb_high_timeout = 75;
                set_low_host();
                while(!is_kb_high());
                set_release_host();
            }
        }
        while(is_host_high());
        set_low_kb();
        TCNT1 = 0;
        while(!is_host_high() && TCNT1 < 20000);  // timeout for button
        set_release_kb();
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
            if(i == -2 && !is_kb_high()) {  // if first stop bit + keyboard SRQ
                set_low_host();
                while(!is_kb_high());
                set_release_host();
            }
        }
        if(is_kb_softpower_pressed()) {
            press_host_softpower();
        }
    }
}

void translation_mode() {
	while(!is_button_activated()) {
		key_event_t ev = transaction_get_keys();
		translate_key_press(ev.keycode_1, ev.pressed_1);
		translate_key_press(ev.keycode_2, ev.pressed_2);
        delay(11);  // like an original adb host
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

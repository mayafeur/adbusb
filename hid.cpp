#include "hid.h"
#include "map.h"

static const uint8_t hid_descriptor[] PROGMEM = {
	// from https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.cpp
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)  // 47
	0x09, 0x06,                    // USAGE (Keyboard)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x02,                    //   REPORT_ID (2)
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)

	0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)

	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x81, 0x03,                    //   INPUT (Cnst,Var,Abs) //

	0x95, 0x06,                    //   REPORT_COUNT (6)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x73,                    //   LOGICAL_MAXIMUM (115) //
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)

	0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x73,                    //   USAGE_MAXIMUM (Keyboard Application) //
	0x81, 0x00,                    //   INPUT (Data,Ary,Abs)

	0xc0,                          // END_COLLECTION
};

key_report_t key_report;

void hid_init() {
	static HIDSubDescriptor node(hid_descriptor, sizeof(hid_descriptor));
	HID().AppendDescriptor(&node);
}

void hid_send_report() {
	HID().SendReport(2, &key_report, sizeof(key_report_t));
}

void hid_press_key(uint8_t hid_code) {
    uint8_t modifier_bitmask = modifier_to_bitmask(hid_code);
    if(modifier_bitmask) {
		key_report.modifiers |= modifier_bitmask;
    }
	else {
		for(int i=0 ; i<6 ; i++) {
			if(key_report.keys[i] == hid_code) {
				return;
			}
		}
		for(int i=0 ; i<6 ; i++) {
			if(key_report.keys[i] == 0) {
				key_report.keys[i] = hid_code;
				break;
			}
		}
	}
	hid_send_report();
}

void hid_release_key(uint8_t hid_code) {
    uint8_t modifier_bitmask = modifier_to_bitmask(hid_code);
    if(modifier_bitmask) {
		key_report.modifiers &= ~modifier_bitmask;
	}
	else {
		for(int i=0 ; i<6 ; i++) {
			if(key_report.keys[i] == hid_code) {
				key_report.keys[i] = 0;
			}
		}
	}
	hid_send_report();
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

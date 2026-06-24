#include <Arduino.h>
#include "gpio.h"

void set_low(uint8_t pin_number) {
    char port = digitalPinToPort(pin_number);
    uint8_t mask = digitalPinToBitMask(pin_number);
	*portModeRegister(port) |= mask;
}

void set_release(uint8_t pin_number) {
    char port = digitalPinToPort(pin_number);
    uint8_t mask = digitalPinToBitMask(pin_number);
	*portModeRegister(port) &= ~mask;
}

bool is_high(uint8_t pin_number) {
    char port = digitalPinToPort(pin_number);
    uint8_t mask = digitalPinToBitMask(pin_number);
    return *portInputRegister(port) & mask;
}

void press_host_softpower() {
    set_low(HOST_SOFTPOWER);
	delay(100);
    set_release(HOST_SOFTPOWER);
}

bool is_button_changed_state() {
    if(!is_high(SWITCH_BUTTON)) {
        while(!is_high(SWITCH_BUTTON));
        return true;
    }
    return false;
}
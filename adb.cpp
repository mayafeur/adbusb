#include <Arduino.h>
#include "adb.h"
#include "gpio.h"

void send_bit_kb(bool bit) {
	set_low(KB_DATA);
	delayMicroseconds(bit ? 35 : 65);
	set_release(KB_DATA);
	delayMicroseconds(bit ? 65 : 35);
	if(!bit) {
		while(!is_high(KB_DATA));  // in case of SRQ from a mouse
	}
}

void send_byte_kb(uint8_t data) {
	for (int i = 7; i >= 0; i--) {
		send_bit_kb(data & (1 << i));
	}
}

void send_attention_sync() {
	set_low(KB_DATA);
	delayMicroseconds(800);
	set_release(KB_DATA);
	delayMicroseconds(70);
}

void send_command(int keyboard_id, int command_id, int register_id) {
	uint8_t command_byte = 0x0000;
	command_byte |= (keyboard_id << 4);
	command_byte |= (command_id << 2);
	command_byte |= (register_id);
	send_byte_kb(command_byte);
}

bool read_bit_kb() {
	while (is_high(KB_DATA));
	TCNT1 = 0;
	while (!is_high(KB_DATA));
	return TCNT1 < 100 ? 1 : 0; // 50us with prescaler 8 on 16 MHz
}

uint16_t read_word_kb() {
	uint16_t word = 0;
	for (int i = 15; i >= 0; i--) {
		word |= (read_bit_kb() << i);
	}
	return word;
}

void reset() {
	set_low(KB_DATA);
	delayMicroseconds(3500);
	set_release(KB_DATA);
	delay(1000);
}

void transaction_set_leds(bool num, bool caps, bool scroll) {
	uint8_t second_byte = 0b00000111;
	second_byte &= ~num;
	second_byte &= ~(caps << 1);
	second_byte &= ~(scroll << 2);
	send_attention_sync();
	send_command(2, 0b10, 2);  // listen register 2
	send_bit_kb(0);
	delayMicroseconds(200);
	send_bit_kb(1);
	send_byte_kb(0x0000);
	send_byte_kb(second_byte);
	send_bit_kb(0);
	send_bit_kb(0);
	delayMicroseconds(200);
}

bool send_wait_stop_to_start() {
	delayMicroseconds(140);  // minimal stop-to-start
	TCNT1 = 0;
	while (is_high(KB_DATA)) {
    	if (TCNT1 > 380 * 2) {  // timeout 260 us + max lasting stop-to-start 120 us (260-140)
			return false;
		}
	}
	return true;
}

uint16_t transaction_get_device_information() {
	send_attention_sync();
	send_command(2, 0b11, 3);  // talk register 3
	send_bit_kb(0);
	if(!send_wait_stop_to_start()) {
		return 0x0000;
	}
	read_bit_kb(); // start bit
	uint16_t word = read_word_kb();  
	read_bit_kb(); // end bit
	return word;
}

key_event_t transaction_get_keys() {
	key_event_t ev = {false, 0, false, 0};
	send_attention_sync();
	send_command(2, 0b11, 0);  // talk register 0
	send_bit_kb(0);
	if(!send_wait_stop_to_start()) {
		return ev;
	}
	read_bit_kb(); // start bit
	uint16_t word = read_word_kb();  
	read_bit_kb(); // end bit
	uint8_t byte1 = (word >> 8) & 0xFF;
    uint8_t byte2 = word & 0xFF;
    ev.pressed_1 = !(byte1 & 0x80);
    ev.keycode_1 = byte1 & 0x7F;
    ev.pressed_2 = !(byte2 & 0x80);
    ev.keycode_2 = byte2 & 0x7F;
	return ev;
}

void transaction_flush_mouse() {
	send_attention_sync();
	send_command(3, 0b00, 1);
	send_bit_kb(0);
}
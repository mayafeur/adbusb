#ifndef ADB_h
#define ADB_h

typedef struct {
	bool pressed_1;
	uint8_t keycode_1;
	bool pressed_2;
	uint8_t keycode_2;
} key_event_t;

void send_bit_kb(bool bit);
void send_byte_kb(uint8_t data);
void send_attention_sync();
void send_command(int keyboard_id, int command_id, int register_id);
bool read_bit_kb();
void reset();
uint16_t read_word_kb();
bool send_wait_stop_to_start();
void transaction_set_leds(bool num, bool caps, bool scroll);
uint16_t transaction_get_device_information();
key_event_t transaction_get_keys();
void transaction_flush_mouse();
void blink_kb();

#endif
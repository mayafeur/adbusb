#ifndef GPIO_h
#define GPIO_h

#define SWITCH_BUTTON 2
#define KB_DATA 10
#define KB_SOFTPOWER 16
#define HOST_DATA 9
#define HOST_SOFTPOWER 8

void set_low(uint8_t pin_number);
void set_release(uint8_t pin_number);
bool is_high(uint8_t pin_number);
void press_host_softpower();
bool is_button_changed_state();

#endif
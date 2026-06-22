#include "adb.h"
#include "conf.h"
#include "hid.h"
#include "main.h"

void setup() {
	// timer for adb timings
	TCCR1A = 0;
    TCCR1B = 0b00000010; // prescaler 8 (2 MHz)
	
	// pull-up for button
	SWITCH_DDR_PORT |= (1 << SWITCH_PIN_NUMBER);
	
	hid_init();
    reset();  // needed only for M0116/M0118
}

void loop() {
	translation_mode();
	passthrough_mode();
}

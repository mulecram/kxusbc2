#include "kx2.h"

#include <avr/cpufunc.h>
#include <avr/io.h>

void kx2_init(void) {
    // Enable interrupt on KX2 power on sense pin (PA3)
    PORTA.PIN3CTRL = PORT_ISC_BOTHEDGES_gc;
}

bool kx2_is_on(void) {
    return (PORTA.IN & PIN3_bm) != 0;
}

void kx2_handle_interrupt(void) {
    // This function can be expanded if any special handling is needed
    // when the KX2 power state changes.
    if (kx2_is_on()) {
        // KX2 powered on - make OSC20M run in standby as we need to be ready
        // to respond to SPI requests within 10 us of RTC_CS being asserted, but
        // the start up time of OSC20M alone is already 12 us.
        ccp_write_io((void*)&(CLKCTRL.OSC20MCTRLA), CLKCTRL_RUNSTDBY_bm);
    } else {
        // KX2 powered off - can disable RUNSTDBY to save power
        ccp_write_io((void*)&(CLKCTRL.OSC20MCTRLA), 0);
    }
}

#include <avr/cpufunc.h>
#include <avr/io.h>

void clock_init(void) {
    // CLKOUT disabled, 20MHz internal oscillator
    ccp_write_io((void*)&(CLKCTRL.MCLKCTRLA), CLKCTRL_CLKSEL_OSC20M_gc);
    // Prescaler disabled
    ccp_write_io((void*)&(CLKCTRL.MCLKCTRLB), 0);
    // LOCKEN enabled
    ccp_write_io((void*)&(CLKCTRL.MCLKLOCK), CLKCTRL_LOCKEN_bm);
    // RUNSTDBY disabled by default (may be enabled later if KX2 is on)
    ccp_write_io((void*)&(CLKCTRL.OSC20MCTRLA), 0);
}

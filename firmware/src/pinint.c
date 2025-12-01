#include <avr/io.h>
#include <avr/interrupt.h>

#include "button.h"
#include "bq.h"
#include "rtc.h"
#include "fsc_pd_ctl.h"
#include "kx2.h"

// Shared ISRs for pin interrupts that concern multiple modules

ISR(PORTA_PORT_vect) {
    if (VPORTA.INTFLAGS & PORT_INT3_bm) {
        kx2_handle_interrupt();
    }
    if (VPORTA.INTFLAGS & PORT_INT4_bm) {
        button_handle_interrupt();
    }
    if (VPORTA.INTFLAGS & PORT_INT5_bm) {
        fsc_pd_notify_interrupt();
    }
    if (VPORTA.INTFLAGS & PORT_INT6_bm) {
        bq_notify_interrupt();
    }
    VPORTA.INTFLAGS = 0xff;
}

ISR(PORTC_PORT_vect) {
    if (VPORTC.INTFLAGS & PORT_INT3_bm) {
        // SPI SS went low
        rtc_handle_spi_ss();
    }
    VPORTC.INTFLAGS = 0xff;
}

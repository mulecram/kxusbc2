#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include "button.h"
#include "debug.h"
#include "rtc.h"

static volatile uint16_t last_button_start_ticks = 0;
static volatile bool button_pressed = false;

static ButtonHandler short_press_handler = 0;

void button_init(void) {
    // Button is connected to PA4, configure as input with pull-up and interrupt on both edges
    PORTA.PIN4CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

void button_set_short_press_handler(ButtonHandler handler) {
    short_press_handler = handler;
}

void button_handle_interrupt(void) {
    // Note: this function is called from an ISR context
    // Button toggled
    if ((PORTA.IN & PIN4_bm) && !button_pressed) {
        // Button pressed
        last_button_start_ticks = rtc_get_ticks();
        button_pressed = true;
    } else if (button_pressed) {
        // Button released
        uint16_t button_press_duration = rtc_get_ticks() - last_button_start_ticks;
        button_pressed = false;
        
        if (button_press_duration > 50 && button_press_duration < 1000) {
            // Short press
            if (short_press_handler) {
                short_press_handler();
            }
        } else if (button_press_duration > 1000) {
            // Long press
            // Reset system
            ccp_write_io((void*)&(RSTCTRL.SWRR), RSTCTRL_SWRE_bm);
        }
    }
}

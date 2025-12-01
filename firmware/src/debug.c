#include <stdio.h>
#include <avr/io.h>
#include <stdarg.h>
#include "rtc.h"

#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#ifdef DEBUG
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
#endif

void debug_init(void) {
#ifdef DEBUG
    PORTMUX.USARTROUTEA |= PORTMUX_USART0_ALT1_gc; // Use alternate pins for USART0 (PA1=TX, PA2=RX)
    PORTA.DIRSET = PIN1_bm; // Set TX pin as output
    USART0.BAUD = (uint16_t)USART_BAUD_RATE(115200); /* set baud rate register */ 

    USART0.CTRLA = 0;
    USART0.CTRLB = USART_RXEN_bm | USART_RXMODE_NORMAL_gc| USART_TXEN_bm;
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;

    stdout = &mystdout;		// define the output stream
#endif
}

char debug_read_char(void) {
#ifdef DEBUG
    if (USART0.STATUS & USART_RXCIF_bm) {
        return USART0.RXDATAL;
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

#ifdef DEBUG
static int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = c;

    // Wait until all data has been transmitted, otherwise we may get garbled output
    // if we enter sleep mode too early.
    while (!(USART0.STATUS & USART_TXCIF_bm));
    USART0.STATUS |= USART_TXCIF_bm; // Clear TXCIF flag
    return 0;
}
#endif

#ifdef DEBUG
void debug_printf(const char *fmt, ...)
{
    uint8_t h, m, s;
    uint16_t ms;
    va_list args;
    rtc_get_time_ms(&h, &m, &s, &ms);
    printf("[%02u:%02u:%02u.%03u] ", h, m, s, ms);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#else
void debug_printf(const char *fmt, ...)
{
    (void)fmt;
}
#endif

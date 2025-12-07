#include <stdio.h>
#include <avr/io.h>
#include <stdarg.h>
#include <util/atomic.h>
#include "rtc.h"
#include "insomnia.h"

#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

// Buffering can be disabled for testing purposes, to prevent wakeups from sleep
// due to the USART interrupts. However, debug_printf will then block, which may
// cause problems with timing in other code.
#define DEBUG_BUFFERED
#define DEBUG_BUFFER_SIZE 256 // Should be a power of two, 256 bytes max.

#ifdef DEBUG
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

#ifdef DEBUG_BUFFERED
static volatile uint8_t tx_buf[DEBUG_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;
#endif

void debug_init(void) {
    PORTMUX.USARTROUTEA |= PORTMUX_USART0_ALT1_gc; // Use alternate pins for USART0 (PA1=TX, PA2=RX)
    PORTA.DIRSET = PIN1_bm; // Set TX pin as output
    USART0.BAUD = (uint16_t)USART_BAUD_RATE(115200); /* set baud rate register */ 

#ifdef DEBUG_BUFFERED
    USART0.CTRLA = USART_TXCIE_bm;
#else
    USART0.CTRLA = 0;
#endif
    USART0.CTRLB = USART_RXEN_bm | USART_RXMODE_NORMAL_gc| USART_TXEN_bm;
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;

    stdout = &mystdout;		// define the output stream
}

char debug_read_char(void) {
    if (USART0.STATUS & USART_RXCIF_bm) {
        return USART0.RXDATAL;
    } else {
        return 0;
    }
}

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }

#ifdef DEBUG_BUFFERED
    uint8_t next_head = (tx_head + 1) % DEBUG_BUFFER_SIZE;
	while (next_head == tx_tail); // Wait for space in buffer
    tx_buf[tx_head] = c;
    tx_head = next_head;
    USART0.CTRLA |= USART_DREIE_bm;
    insomnia_mask |= INSOMNIA_DEBUG_TX;
#else
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = c;
    while (!(USART0.STATUS & USART_TXCIF_bm));
    USART0.STATUS |= USART_TXCIF_bm; // Clear TXCIF flag
#endif
    return 0;
}

void debug_printf(const char *fmt, ...) {
    va_list args;
    printf("[%u] ", rtc_get_ticks());
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

#ifdef DEBUG_BUFFERED
ISR(USART0_DRE_vect) {
	if (tx_tail != tx_head) {
		USART0.TXDATAL = tx_buf[tx_tail];
        tx_tail = (tx_tail + 1) % DEBUG_BUFFER_SIZE;
	} else {
        // Nothing more to send; disable interrupt
		USART0.CTRLA &= ~USART_DREIE_bm;
	}
}

ISR(USART0_TXC_vect) {
    // Transmission complete
    if (tx_tail == tx_head) {
        // No more data to send
        insomnia_mask &= ~INSOMNIA_DEBUG_TX;
    }
    USART0.STATUS |= USART_TXCIF_bm; // Clear interrupt flag
}
#endif

#else
// Dummy implementations when DEBUG is not defined
void debug_init(void) {
    // No-op
}

char debug_read_char(void) {
    return 0;
}

void debug_printf(const char *fmt, ...) {
    (void)fmt;
}

#endif

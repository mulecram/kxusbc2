#include <stdio.h>
#include <avr/io.h>
#include <stdarg.h>
#include <util/atomic.h>
#include "rtc.h"
#include "insomnia.h"

#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#define DEBUG_BUFFER_SIZE 256 // Should be a power of two, 256 bytes max.

#ifdef DEBUG
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
#endif

static volatile uint8_t tx_buf[DEBUG_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

void debug_init(void) {
#ifdef DEBUG
    PORTMUX.USARTROUTEA |= PORTMUX_USART0_ALT1_gc; // Use alternate pins for USART0 (PA1=TX, PA2=RX)
    PORTA.DIRSET = PIN1_bm; // Set TX pin as output
    USART0.BAUD = (uint16_t)USART_BAUD_RATE(115200); /* set baud rate register */ 

    USART0.CTRLA = USART_TXCIE_bm;
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

    uint8_t next_head = (tx_head + 1) % DEBUG_BUFFER_SIZE;
	while (next_head == tx_tail); // Wait for space in buffer
    tx_buf[tx_head] = c;
    tx_head = next_head;
    USART0.CTRLA |= USART_DREIE_bm;
    insomnia_mask |= INSOMNIA_DEBUG_TX;
}
#endif

#ifdef DEBUG
void debug_printf(const char *fmt, ...) {
    va_list args;
    printf("[%u] ", rtc_get_ticks());
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#else
void debug_printf(const char *fmt, ...) {
    (void)fmt;
}
#endif

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


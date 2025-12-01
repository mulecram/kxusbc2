#pragma once

#include <stdint.h>

void rtc_init(void);
void rtc_get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void rtc_get_time_ms(uint8_t *phours, uint8_t *pminutes, uint8_t *pseconds, uint16_t *pmilliseconds);

// Returns the current RTC ticks, which increment at 1024 Hz, so this value is close
// to a rolling millisecond value and can be used in cases where the difference between
// 1000 or 1024 ms to a second is not important or can be compensated.
uint16_t rtc_get_ticks(void);

// Set an alarm to trigger an interrupt in a specified number of ticks.
// This can be used to wake up the system from low-power mode.
void rtc_set_alarm(uint16_t ticks);

void rtc_handle_spi_ss(void);

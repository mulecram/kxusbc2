#pragma once

#include <stdint.h>

uint8_t decimalToBcd(uint8_t val);
uint8_t bcdToDecimal(uint8_t val);
int16_t measure_chip_temperature(void);

#pragma once

#include <stdint.h>

#define INSOMNIA_DEBUG_TX (1 << 0)
#define INSOMNIA_RTC_SPI  (1 << 1)
#define INSOMNIA_FSC_PD   (1 << 2)

extern volatile uint8_t insomnia_mask;

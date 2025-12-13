#pragma once

#include <stdint.h>
#include <stdbool.h>

#define USERROW_MAGIC 0x5255

struct UserRow {
    uint16_t magic;             // must be 0x5255 to indicate valid user row
    int8_t factoryRtcOffset;    // factory RTC offset in ppm, -127 to 126
};

extern struct UserRow *userrow;

bool userrow_valid(void);

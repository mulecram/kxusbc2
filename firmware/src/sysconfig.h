#pragma once

#include <avr/eeprom.h>
#include <stdbool.h>

#define SYSCONFIG_MAGIC 0x4355

enum Role {
    SRC = 0,
    SNK = 1,
    DRP = 2,
    TRY_SRC = 3,
    TRY_SNK = 4
} __attribute__ ((__packed__));

enum PDMode {
    PD_OFF = 0,
    PD_2_0 = 1,
    PD_3_0 = 2
} __attribute__ ((__packed__));

struct SysConfig {
    uint16_t magic;                   // must be 0x4355 to indicate valid config
    enum Role role;
    enum PDMode pdMode;
    uint16_t chargingCurrentLimit;    // mA, range 50-5000
    uint16_t chargingVoltageLimit;    // mV, range 10000-18800
    uint16_t dcInputCurrentLimit;     // mA, input from DC jack (VAC2), range 100-3300
    uint16_t otgCurrentLimit;         // mA, in OTG mode, range 120-3320
    uint16_t dischargingVoltageLimit; // mV, min. battery voltage for OTG mode
    uint16_t otgVoltageHeadroom;      // mV, voltage to be added in OTG mode to compensate for drop
    bool chargeWhenRigIsOn;
    bool enableThermistor;
    int16_t userRtcOffset;            // user RTC offset in ppm, set via KX2 RTC ADJ menu (-278 to +273)
};

extern struct SysConfig *sysconfig;

bool sysconfig_valid(void);
void sysconfig_update_word(void *addr, uint16_t value);

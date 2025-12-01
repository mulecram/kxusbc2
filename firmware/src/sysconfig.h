#pragma once

#include <avr/eeprom.h>
#include <stdbool.h>

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
    enum Role role;
    enum PDMode pdMode;
    uint16_t chargeCurrentLimit;    // mA, range 50-5000
    uint16_t maxChargeVoltage;      // mV, range 10000-13990
    uint16_t dcInputCurrentLimit;   // mA, input from DC jack (VAC2)
    uint16_t otgCurrentLimit;       // mA, in OTG mode, range 120-3320
    bool chargeWhenRigIsOn;
    bool enableThermistor;
    int8_t rtcOffset;               // RTC offset in ppm
};

extern struct SysConfig sysconfig;

void sysconfig_read(void);
void sysconfig_write(void);

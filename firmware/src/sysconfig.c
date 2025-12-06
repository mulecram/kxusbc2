#include <avr/eeprom.h>

#include "sysconfig.h"

static EEMEM struct SysConfig sysconfig_eeprom = {
    .role = DRP,
    .pdMode = PD_2_0,
    .chargeCurrentLimit = 3000,
    .maxChargeVoltage = 12600,
    .dcInputCurrentLimit = 3000,
    .otgCurrentLimit = 3000,
    .chargeWhenRigIsOn = false,
    .enableThermistor = false,
    .rtcOffset = 0
};

struct SysConfig sysconfig;

void sysconfig_read(void) {
    eeprom_read_block(&sysconfig, &sysconfig_eeprom, sizeof(sysconfig));
}

void sysconfig_write(void) {
    eeprom_write_block(&sysconfig, &sysconfig_eeprom, sizeof(sysconfig));
}

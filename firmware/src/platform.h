/* This file provides the "glue" between the FSC PD code and our platform. It implements several defines
   and functions that are called/expected by the PD code. */
#pragma once

#include "fsc_pd/FSCTypes.h"

#define TICK_SCALE_TO_MS    1.024   /* RTC runs at 1024 Hz */
#define NUM_PORTS           1

typedef enum
{
    VBUS_LVL_5V,
    VBUS_LVL_HV,
    VBUS_LVL_ALL
} VBUS_LVL;

typedef enum
{
    SINK = 0,
    SOURCE
} SourceOrSink;

/* Observable event types */
typedef enum
{
    CC1_ORIENT         = 0x00000001,
    CC2_ORIENT         = 0x00000002,
    CC_NO_ORIENT       = 0x00000004,
    CC_ORIENT_ALL      = CC1_ORIENT | CC2_ORIENT | CC_NO_ORIENT,
    PD_NEW_CONTRACT    = 0x00000008,
    PD_NO_CONTRACT     = 0x00000010,
    PD_CONTRACT_ALL    = PD_NEW_CONTRACT | PD_NO_CONTRACT,
    PD_STATE_CHANGED   = 0x00000020,
    ACC_UNSUPPORTED    = 0x00000040,
    DATA_ROLE          = 0x00000080,
    BIST_DISABLED      = 0x00000100,
    BIST_ENABLED       = 0x00000200,
    BIST_ALL           = BIST_ENABLED | BIST_DISABLED,
    ALERT_EVENT        = 0x00000400,
    EVENT_ALL          = 0xFFFFFFFF,
} Events_t;

void platform_set_vbus_lvl_enable(FSC_U8 port, VBUS_LVL level, FSC_BOOL enable,
                                  FSC_BOOL disableOthers);

void platform_set_pps_voltage(FSC_U8 port, FSC_U16 mv);

void platform_set_pps_current(FSC_U8 port, FSC_U16 ma);

void platform_set_vbus_discharge(FSC_U8 port, FSC_BOOL enable);

void platform_set_vconn(FSC_U8 port, FSC_BOOL enable);

FSC_BOOL platform_get_device_irq_state(FSC_U8 port);

FSC_BOOL platform_i2c_write(FSC_U8 SlaveAddress,
                            FSC_U8 RegAddrLength,
                            FSC_U8 DataLength,
                            FSC_U8 PacketSize,
                            FSC_U8 IncSize,
                            FSC_U8 RegisterAddress,
                            FSC_U8* Data);

FSC_BOOL platform_i2c_read(FSC_U8 SlaveAddress,
                           FSC_U8 RegAddrLength,
                           FSC_U8 DataLength,
                           FSC_U8 PacketSize,
                           FSC_U8 IncSize,
                           FSC_U8 RegisterAddress,
                           FSC_U8* Data);

void platform_delay_10us(FSC_U8 delayCount);

FSC_U16 platform_get_system_time(void);

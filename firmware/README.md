# Firmware notes

The firmware is designed to be built with AVR-GCC.

Prerequisites:

* AVR-GCC toolchain (version 15.2.0)
* AVRDUDE (for programming)

Instructions to build:

1. Download release v4.1.2 of the FUSB302 reference code from onsemi (login required): https://www.onsemi.com/design/evaluation-board/FUSB302BGEVB
2. Place the downloaded file `FUSB302 REFERENCE CODE.ZIP` in the root directory of the repository.
3. `sh merge_fsc_pd.sh`
4. Edit `Makefile` and set `PROGRAMMER` and `PORT` as needed.
4. `make` to compile the code.
5. `make flash` to flash the AVR.
6. `make eeprom` to write the default sysconfig settings to the EEPROM.


## USB PD support
USB Power Delivery (PD) is a pretty complex specification/protocol that involves sending messages between the source and the sink (over the CC lines of the USB-C connector) to exchange capabilities and negotiate a power profile. Implementing a PD protocol stack from scratch for a DRP (Dual Role Port) would be a major undertaking. With some optimizations, I have succeeded in squeezing the reference code supplied by onsemi (makers of the FUSB302B IC) into the 32 KB flash of the ATtiny3226, and making it work.

The downside is that due to the license, while the reference code may be used freely in conjunction with onsemi chips, it cannot be republished. Therefore, this repository does not include the complete firmware source code, and requires downloading the reference code from onsemi and applying a patch with my optimizations before the firmware can be compiled (see instructions above).

### Modifications to the onsemi reference code
The patch included in the repository, `fsc_pd.patch`, which is applied automatically by `merge_fsc_pd.sh`, makes the following changes to the reference code:

- Replaced `FSC_U32` with `FSC_U16` or `FSC_U8` where bigger values would never be used anyway. As the AVR is an 8-bit device, operations with larger integers are expensive in terms of code size, memory usage and execution speed, especially if multiplication or division are involved.
    - This shaves around 1700 bytes from the compiled code size.
- When PD 3.0 support is enabled, devices that send a “Get Source Capabilities Extended” message trigger a hardware bug in the FUSB302B, which doesn't automatically send a GoodCRC for this particular message type.
    - Devices that send such messages then run into a timeout because they don't get the expected GoodCRC, and a hard reset loop ensues.
    - The onsemi reference code has a software workaround for this, enabled by the `FSC_GSCE_FIX` define. It works by sending a manual GoodCRC in software for such messages. As the GoodCRC timeout is 1 ms (tReceive), this has to happen fast.
    - However, there is also a bug in the code enabled by `FSC_GSCE_FIX`. Specifically, it uses the `I_CRC_CHK` interrupt instead of the normal `I_GCRCSENT` to trigger receiving a packet in `PDProtocol.c:ProtocolIdle()`. The latter can also be called from `ProtocolSendingMessage()`, but before making that call, `ProtocolSendingMessage()` already checks if `I_TXSENT` or `I_CRC_CHK` are set, and if any of them are, then it also clears `I_CRC_CHK`. This causes `ProtocolIdle()` to miss the incoming packet, which will only be processed when the next packet comes in – usually too late, so timers expire and hard resets are made. The usual case where this is triggered is when the PD source rejects a request, and doesn't send any further packets. Then the tSenderResponse timeout kicks in, the code issues a hard reset, and this loops forever.
    - The patch comments out the clearing of the `I_CRC_CHK` bit, which doesn't appear to have negative consequences.
- Increased tSenderResponse to 32 ms (USB PD ECN “Chunking Timing Issue”).


## Programming/Debugging
The ATtiny3226 has a one-wire UPDI interface for programming and debugging. The KXUSBC2 board provides this on a 3-pin 2.54 mm header on the backside, along with Vcc (3.3 V) and GND. The header doesn't actually need to be soldered – the middle pad is slightly offset/staggered, so one can get a good enough interference fit by simply sticking a pin header through the holes for a temporary connection to the debugger. Call it a poor man's (or hobbyist-friendly) Tag-Connect 😂 This can be used directly with a NanoUPDI or a UPDI Friend, thus giving a programming solution for less than $10.

There is also a serial interface on a separate 3-pin header (also staggered), wired to the MCU's hardware USART, as UPDI does not support debug console output. Note that the levels there are 3.3 V, not RS-232.


## RTC emulation
The firmware acts as an SPI slave/client, emulating PCF2123-style registers to the KX2 firmware. Only the set of registers actually used by the KX2 firmware is implemented. The RTC peripheral of the MCU is clocked by an external 32.768 kHz crystal.

A fixed offset (in ppm) can be set in the sysconfig/EEPROM to calibrate each board.

The KX2 has an "RTC ADJ" menu that lets the user compensate for a clock being too slow or too fast, by setting the number of seconds per day to compensate. The KX2 firmware translates this into a correction value for the PCF2123. The RTC emulation in the KXUSBC2 firmware calculates the equivalent ppm correction (4.34 ppm per unit according to the PCF2123 datasheet, in the "course mode" that the KX2 uses), and applies it to the `RTC.CALIB` register of the ATtiny3226. The maximum correction that can be applied this way is ±127 ppm (about 11 seconds per day). Larger values set in the RTC ADJ menu will be clamped to this range.

The RTC emulation also features a temperature compensation. Once a minute, it measures the temperature using the MCU's built-in sensor and calculates an offset according to the temperature coefficient and turnover temperature given in the crystal's datasheet.


## Input priority
The charger uses either the external DC jack input (E pad), or USB, whichever is connected first. If both sources are connected when the charger starts up, it prefers the DC jack. The charger keeps using the current source as long as it is available, even if another source becomes available. However, if the current source disconnects, the charger switches. For example, if you connect a DC supply while the charger is charging from USB, it will keep using USB. If you then disconnect USB, it will seamlessly switch over to the DC jack input.

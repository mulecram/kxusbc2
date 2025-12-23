#!/bin/sh

# Factory calibration for RTC. Flash firmware built with -DRTC_CALIBRATION_MODE, then
# measure frequency on PA2 (= RX pin on serial header).
# This script will flash the rtc_calib.hex firmware, then prompt for measured frequency,
# and finally program the RTC calibration offset into the user row.

# Extract avrdude configuration from Makefile
PROGRAMMER=$(grep "^PROGRAMMER" Makefile | cut -d' ' -f3)
MCU=$(grep "^MCU" Makefile | cut -d' ' -f3)
PORT=$(grep "^PORT" Makefile | cut -d' ' -f3)

# Step 1: Program EEPROM and fuses
echo "Programming EEPROM and fuses..."
make eeprom fuses

# Step 2: Flash the calibration firmware
echo "Flashing rtc_calib.hex firmware..."
avrdude -c $PROGRAMMER -p $MCU -P $PORT -V -U flash:w:rtc_calib.hex:i

if [ $? -ne 0 ]; then
    echo "Error: Failed to flash firmware"
    exit 1
fi

echo "Firmware flashed successfully"
echo ""
echo "Please measure the frequency on PA2 (RX pin on serial header)"
echo "and enter the measured frequency in Hz (e.g., 512.032):"
read measured_freq

if [ -z "$measured_freq" ]; then
    echo "Error: No frequency provided"
    exit 1
fi

# Calculate offset in ppm: offset_ppm = 1000000 * (measured_freq/512 - 1)
offset_ppm=$(awk "BEGIN {printf \"%.0f\", 1000000 * ($measured_freq / 512 - 1)}")

# Clamp to int8_t range (-128 to 127)
if [ "$offset_ppm" -gt 127 ]; then
    offset_ppm=127
    echo "Warning: offset clamped to 127"
elif [ "$offset_ppm" -lt -127 ]; then
    offset_ppm=-127
    echo "Warning: offset clamped to -127"
fi

echo "Measured frequency: $measured_freq Hz"
echo "Calculated offset: $offset_ppm ppm"

# Convert to hex (as signed int8)
if [ "$offset_ppm" -lt 0 ]; then
    offset_hex=$(printf '%02X' $((256 + $offset_ppm)))
else
    offset_hex=$(printf '%02X' "$offset_ppm")
fi

echo "Offset (hex): 0x$offset_hex"

# Step 3: Program user row with: byte0=0x55, byte1=0x52, byte2=offset_ppm
echo "Programming user row..."
echo "Using: -c $PROGRAMMER -p $MCU"
avrdude -c $PROGRAMMER -p $MCU -P $PORT -U userrow:w:0x55,0x52,0x$offset_hex:m

if [ $? -ne 0 ]; then
    echo "Error: Failed to program user row"
    exit 1
fi

echo "User row programmed successfully"
echo ""

# Step 4: Verify calibration
echo "Please measure the frequency on PA2 again to verify the calibration."
echo "Note: a long gate time of about 10 seconds should be used, as the MCU"
echo "applies the calibration gradually."
echo "Press Enter to continue after measurement."
read confirm

# Step 5: Flash release firmware
echo "Flashing release firmware..."
make flash

if [ $? -ne 0 ]; then
    echo "Error: Failed to flash firmware"
    exit 1
fi

echo "Done!"

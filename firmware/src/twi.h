#pragma once
    
#include <stdbool.h>
#include <stdint.h>

void twi_init(void);

bool twi_send_byte(uint8_t addr, uint8_t data);
bool twi_send_bytes(uint8_t addr, uint8_t* data, uint8_t len);
bool twi_send_reg_bytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len);

bool twi_read_byte(uint8_t addr, uint8_t* data);
bool twi_read_bytes(uint8_t addr, uint8_t* data, uint8_t len);

bool twi_send_and_read_bytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len);

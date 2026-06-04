#pragma once
#include <stdint.h>

uint8_t i2c_ddr_setup(void);
uint8_t i2c_port_setup(void);
uint8_t i2c_write(uint8_t addr, uint8_t *buff, uint8_t size);
uint8_t i2c_read(uint8_t addr, uint8_t *buff, uint8_t size);
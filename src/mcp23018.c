#include "mcp23018.h"
#include "mcp23018_internal.h"
#include "i2c.h"

static void _write1(uint8_t addr, uint8_t data) {
  uint8_t buff[] = {addr, data};
  i2c_write(MCP23018_ADDR, buff, 2);
}

static void _write2(uint8_t addr, uint8_t data_a, uint8_t data_b) {
  uint8_t buff[] = {addr, data_a, data_b};
  i2c_write(MCP23018_ADDR, buff, 3);
}

void mcp23018_reset(void) {
  _write1(ADDR_IOCON, 0b000000);
  _write2(ADDR_IODIR, 0b000000, 0b000000);
}

void mcp23018_set(uint8_t port_a, uint8_t port_b) {
  _write2(ADDR_GPIO, port_a, port_b);
}
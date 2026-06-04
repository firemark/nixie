#include "i2c.h"
#include <avr/io.h>

static inline void nop() {
  __asm__ volatile("nop");
}

uint8_t i2c_ddr_setup(void) {
  return (1 << DDB0) | (1 << DDB2);
}

uint8_t i2c_port_setup(void) {
  // B0 as SDA, B2 as SCL.
  return (1 << PB0) | (1 << PB2);
}

#define scl_high() \
  { PORTB |= (1 << PB2); }
#define scl_low() \
  { PORTB &= ~(1 << PB2); }
#define sda_input()       \
  {                       \
    DDRB &= ~(1 << DDB0); \
    PORTB |= (1 << PB0);  \
  }
#define sda_output() \
  { DDRB |= (1 << DDB0); }
#define sda_high() \
  { PORTB |= (1 << PB0); }
#define sda_low() \
  { PORTB &= ~(1 << PB0); }
#define is_sda_high() (PINB & (1 << PINB0))

static uint8_t i2c_read_ack() {
  uint8_t retval = 0;
  scl_low();
  sda_input();
  scl_high();
  nop();
  nop();
  if (is_sda_high()) {  // Error.
    retval = 1;
  }
  scl_low();
  return retval;
}

static void i2c_write_addr(uint8_t addr) {
  sda_low();

  for (uint8_t i = 0; i < 7; i++) {
    scl_low();
    if (addr & (1 << 6)) {
      sda_high();
    } else {
      sda_low();
    }
    scl_high();
    addr <<= 1;
  }
}

static void i2c_finish() {
  sda_low();
  scl_high();
  nop();
  nop();
  nop();
  sda_high();
}

uint8_t i2c_write(uint8_t addr, uint8_t* buff, uint8_t size) {
  uint8_t retval = 0;

  i2c_write_addr(addr);

  {
    // Write bit
    scl_low();
    sda_low();
    nop();
    nop();
    scl_high();
  }

  retval |= i2c_read_ack();
  sda_output();

  for (uint8_t j = 0; j < size; j++) {
    uint8_t cmd = buff[j];
    for (uint8_t i = 0; i < 8; i++) {
      scl_low();
      if (cmd & (1 << 7)) {
        sda_high();
      } else {
        sda_low();
      }
      scl_high();
      cmd <<= 1;
    }

    retval |= i2c_read_ack();
    sda_output();
  }

  i2c_finish();

  return retval;
}

uint8_t i2c_read(uint8_t addr, uint8_t* buff, uint8_t size) {
  uint8_t retval = 0;

  i2c_write_addr(addr);

  {
    // Read bit
    scl_low();
    sda_high();
    scl_high();
  }

  retval |= i2c_read_ack();

  scl_low();
  for (uint8_t j = 0; j < size; j++) {
    uint8_t* byte = &buff[j];
    *byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
      scl_high();
      *byte <<= 1;
      if (is_sda_high()) {
        *byte |= 1;
      }
      scl_low();
    }

    {
      // Write ACK
      sda_output();
      sda_high();
      scl_high();
      nop();
      nop();
      scl_low();
    }
  }

  i2c_finish();

  return retval;
}

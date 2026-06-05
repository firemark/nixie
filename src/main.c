#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "i2c.h"
#include "mcp23018.h"
#include "rtc.h"

#ifdef TEST
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "attiny13");
// AVR_MCU_VCD_FILE("main.vcd", 1000);
// AVR_MCU_EXTERNAL_PORT_PULL('B', (1 << 0), 1);
// AVR_MCU_EXTERNAL_PORT_PULL('B', (1 << 1), 0xFFF);

// const struct avr_mmcu_vcd_trace_t _mytrace[] _MMCU_ = {
//     {AVR_MCU_VCD_SYMBOL("SDA_DIR"), .mask = (1 << 0), .what = (void*)&DDRB},
//     {AVR_MCU_VCD_SYMBOL("SDA_IN"), .mask = (1 << 0), .what = (void*)&PINB},
//     {AVR_MCU_VCD_SYMBOL("SDA_OUT"), .mask = (1 << 0), .what = (void*)&PORTB},
//     {AVR_MCU_VCD_SYMBOL("SCL"), .mask = (1 << 2), .what = (void*)&PORTB},
//     {AVR_MCU_VCD_SYMBOL("INT"), .mask = (1 << 1), .what = (void*)&PINB},
//     {AVR_MCU_VCD_SYMBOL("MOTOR_A"), .mask = (1 << 3), .what = (void*)&PORTB},
//     {AVR_MCU_VCD_SYMBOL("MOTOR_B"), .mask = (1 << 4), .what = (void*)&PORTB},
// };
#endif

#define IS_INT_PIN_HIGH (PINB & (1 << PB1))
#define IS_MINUTE_BTN_HIGH (PINB & (1 << PB3))
#define IS_HOUR_BTN_HIGH (PINB & (1 << PB4))
#define SELECT_TIME_US 500

static struct Time time;

static void setup() {
  // Input pin B1 with pull-up
  PORTB = (1 << PB1) | (1 << PB3) | (1 << PB4) | i2c_port_setup();
  DDRB = i2c_ddr_setup();

  mcp23018_reset();

  if (rtc_lost_power()) {
    rtc_reset();
  }
}

static void set_time_to_nixie() {
  mcp23018_set(0xF0 | time.hours, 0xFF);
  _delay_us(SELECT_TIME_US);
  mcp23018_set(0x0F | time.hours, 0xFF);
  _delay_us(SELECT_TIME_US);
  mcp23018_set(0xFF, 0xF0 | time.minutes);
  _delay_us(SELECT_TIME_US);
  mcp23018_set(0xFF, 0x0F | time.minutes);
  _delay_us(SELECT_TIME_US);
  mcp23018_set(0xFF, 0xFF);
}

static void loop() {
  static uint8_t ticks = 0;
  if (!IS_INT_PIN_HIGH) {
    rtc_clear_alarm();
    time = rtc_get_time();
  }

  if (!IS_HOUR_BTN_HIGH) {
    if (ticks++ > 100) {
      rtc_inc_hour(&time);
      rtc_set_time(&time);
      ticks = 0;
    }
  } else if (!IS_MINUTE_BTN_HIGH) {
    if (ticks++ > 100) {
      rtc_inc_minute(&time);
      rtc_set_time(&time);
      ticks = 0;
    }
  } else {
    ticks = 0;
  }
}

int main() {
  setup();
  time = rtc_get_time();
  for (;;) {
    loop();
    set_time_to_nixie();
  }
  return 0;
}

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

static void setup() {
  // Input pin B1 with pull-up
  PORTB = (1 << PB1) | (1 << PB3) | (1 << PB4) | i2c_port_setup();
  DDRB = i2c_ddr_setup();

  GIMSK = (1 << INT0) | (1 << PCIE);  // Interrupts on.
  PCMSK = (1 << PCINT3) | (1 << PCINT4);  // Interrupts on for PCINT.

  mcp23018_reset();

  if (rtc_lost_power()) {
    rtc_reset();
  }
}

static void set_time_to_nixie(struct Time * time) {
  mcp23018_set(time->hours, time->minutes);
}


static uint8_t loop(struct Time * time) {
  if (!IS_INT_PIN_HIGH) {
    rtc_clear_alarm();
    *time = rtc_get_time();
  }

  uint8_t try_again = 0;
  if (!IS_HOUR_BTN_HIGH) {
    try_again = 1;
    rtc_inc_hour(time);
    rtc_set_time(time);
  } else if (!IS_MINUTE_BTN_HIGH) {
    try_again = 1;
    rtc_inc_minute(time);
    rtc_set_time(time);
  } 

  set_time_to_nixie(time);
  return try_again;
}

EMPTY_INTERRUPT(INT0_vect);
EMPTY_INTERRUPT(PCINT0_vect);

int main() {
  setup();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  struct Time time = rtc_get_time();
  set_time_to_nixie(&time);

  for (;;) {
    cli();
    while (loop(&time)) {
      _delay_ms(250);
    }
    sei();
    if (IS_INT_PIN_HIGH) {
      sleep_cpu();
    }
  }
  return 0;
}

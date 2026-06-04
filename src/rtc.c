#include "rtc.h"
#include "i2c.h"
#include "rtc_internal.h"

const uint8_t RESET_DATA[][2] = {
    // set time to 08:08:00
    {TIME_SECONDS_ADDR, 0},
    {TIME_MINUTES_ADDR, 8},
    {TIME_HOURS_ADDR, 8 | 1 << 6},

    // set alarm to 00:00:00 + ALARM every second
    {ALARM1_SECONDS_ADDR, ALARM_FLAG | 0},
    {ALARM1_MINUTES_ADDR, ALARM_FLAG | 0},
    {ALARM1_HOURS_ADDR, ALARM_FLAG | 1 << 6},
    {ALARM1_DAYS_ADDR, ALARM_FLAG | 1},

    {CONTROL_ADDR, 0b00000101},  // Set INT pin + set alarm1.
    {STATUS_ADDR, 0b00000000},   // Reset oscillator stop flag.
};

static void _rtc_write(uint8_t addr, uint8_t data) {
  uint8_t buff[] = {addr, data};
  i2c_write(RTC_ADDR, buff, 2);
}

static uint8_t _rtc_read(uint8_t addr) {
  uint8_t data;
  i2c_write(RTC_ADDR, &addr, 1);
  i2c_read(RTC_ADDR, &data, 1);
  return data;
}

void rtc_reset(void) {
  uint8_t i;
  for (i = 0; i < sizeof(RESET_DATA) / sizeof(RESET_DATA[0]); i++) {
    _rtc_write(RESET_DATA[i][0], RESET_DATA[i][1]);
  }
}

uint8_t rtc_lost_power(void) {
  uint8_t status = _rtc_read(STATUS_ADDR);
  return status & RTC_STATUS_FLAG;
}

struct Time rtc_get_time(void) {
  struct Time time = {
    .hours = _rtc_read(TIME_HOURS_ADDR) & 0b00111111,
    .minutes = _rtc_read(TIME_MINUTES_ADDR) & 0b01111111,
  };
  return time;
}

void rtc_set_time(struct Time * time) {
  _rtc_write(TIME_MINUTES_ADDR, time->minutes);
  _rtc_write(TIME_HOURS_ADDR, time->hours | (1 << 6));
  _rtc_write(TIME_SECONDS_ADDR, 0);
}

void rtc_inc_minute(struct Time * time) {
  uint8_t minutes = time->minutes;
  switch(minutes & 0b1111) {
    case 9: // X9
      minutes &= 0b1110000; // X0
      if (minutes >= 0b1010000) { // 50
        minutes = 0; // 00
      } else {
        minutes += 0b0010000; // +10
      }
    break;
    default: 
      minutes += 0b000001; // +01
    break;
  }
  time->minutes = minutes;
}

void rtc_inc_hour(struct Time * time) {
  uint8_t hours = time->hours;
  switch(hours & 0b1111) {
    case 3: // X3
      if ((hours & 0b110000) >= 0b0100000) { // 23
        hours = 0; // 00
      } else {
        hours += 0b00000001; // +01
      }
      break;
    case 9: // X9
      hours &= 0b110000; //  X0
      hours += 0b010000; // +10
      break;
    default:
      hours += 0b000001; // +01
    break;
  }
  time->hours = hours;
}

void rtc_clear_alarm(void) {
  _rtc_write(STATUS_ADDR, _rtc_read(STATUS_ADDR) & 0b11111110);
}
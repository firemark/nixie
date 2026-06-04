#pragma once
#define TIME_SECONDS_ADDR 0x00
#define TIME_MINUTES_ADDR 0x01
#define TIME_HOURS_ADDR 0x02
#define ALARM1_SECONDS_ADDR 0x07
#define ALARM1_MINUTES_ADDR 0x08
#define ALARM1_HOURS_ADDR 0x09
#define ALARM1_DAYS_ADDR 0x0A
#define ALARM2_MINUTES_ADDR 0x0B
#define ALARM2_HOURS_ADDR 0x0C
#define ALARM2_DAYS_ADDR 0x0D
#define CONTROL_ADDR 0x0E
#define STATUS_ADDR 0x0F
#define RTC_ADDR 0x68

#define RTC_STATUS_FLAG 1 << 7
#define ALARM_FLAG 1 << 7

#define _rtc_24_encode(val) ((val % 10) | (((val / 10) % 2) << 4))
#define _rtc_60_encode(val) ((val % 10) | (((val / 10) % 6) << 4))
#define _rtc_24_decode(val) ((val & 0b1111) + ((val >> 4) & 0b1) * 10)
#define _rtc_60_decode(val) ((val & 0b1111) + ((val >> 4) & 0b111) * 10)
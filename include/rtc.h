#pragma once
#include <stdint.h>

struct Time {
    uint8_t hours;
    uint8_t minutes;
};

/// @brief Resets RTC.
void rtc_reset(void);

uint8_t rtc_lost_power(void);

/// @brief Increase time by one minute.
/// @param time
void rtc_inc_minute(struct Time * time);

/// @brief Increase time by one hour.
/// @param time
void rtc_inc_hour(struct Time * time);

/// @brief Sets time in time register.
/// @param time
void rtc_set_time(struct Time * time);

/// @brief Clears interupt flag in alarm2 register.
void rtc_clear_alarm(void);

/// @brief Returns time from time register.
/// @return time.
struct Time rtc_get_time(void);
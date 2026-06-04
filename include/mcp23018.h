#pragma once
#include <stdint.h>

/// @brief Resets MPC.
void mcp23018_reset(void);

/// @brief Set value for specify register
void mcp23018_set(uint8_t port_a, uint8_t port_b);
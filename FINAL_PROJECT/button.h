#pragma once

#include "nrf.h"
#include "stdbool.h"

typedef enum {
  GPIO_INPUT = 0,
  GPIO_OUTPUT,
} gpio_direction_t;

void gpio_config(uint8_t gpio_num, gpio_direction_t dir);

void gpio_clear(uint8_t gpio_num);

bool gpio_read(uint8_t gpio_num);

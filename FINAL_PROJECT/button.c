#include <stdio.h>
#include "button.h"

static int check_button(void) {
    return 1;
}

typedef struct{
  // Step 3:
  // Add register definitions here
  uint32_t unused_A[321];
  uint32_t out;
  uint32_t outset;
  uint32_t outclr;
  uint32_t in;
  uint32_t dir;
  uint32_t dirset;
  uint32_t dirclr;
  uint32_t latch;
  uint32_t detect_mode;
  uint32_t unused_B[118];
  uint32_t pin_cnf[32];
} gpio_reg_t;

volatile gpio_reg_t* gpio_pins = (gpio_reg_t*)(0x50000000);
volatile gpio_reg_t* gpio_pins_2 = (gpio_reg_t*)(0x50000300);

void gpio_config(uint8_t gpio_num, gpio_direction_t dir) {
  uint32_t port = gpio_num >> 5;
  uint32_t pin = gpio_num & 0X1F;

  if (port == 0) {
    if (dir == GPIO_INPUT) {
      gpio_pins->pin_cnf[pin] = 0x00300001;
    } else {
      gpio_pins->pin_cnf[pin] = 0x00300003;
    }
  } else {
    if (dir == GPIO_INPUT) {
      gpio_pins_2->pin_cnf[pin] = 0x00300001;
    } else {
      gpio_pins_2->pin_cnf[pin] = 0x00300003;
    }
  }
}

void gpio_clear(uint8_t gpio_num) {
  uint32_t port = gpio_num >> 5;
  uint32_t pin = gpio_num & 0x1F;

  if (port == 0) {
    gpio_pins->dir = 0;
    gpio_pins->out = 0;
  } else {
    gpio_pins_2->dir = 0;
    gpio_pins_2->out = 0;
  }
}

bool gpio_read(uint8_t gpio_num) {
  int32_t port = gpio_num >> 5;
  uint32_t pin = gpio_num & 0x1F;

  uint32_t state;
  state = (gpio_pins->in >> pin) & 0x1;
  return state == 1;
}
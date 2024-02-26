// App Timer example app
//
// Use the App Timer to blink LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"

#include "microbit_v2.h"
#include "button.h"
#include "mic.h"

int button_pressed() {
  return (!gpio_read(14));
}

int shout() {
  uint32_t avg = microphone();

  return (avg > 15000);
}

int main(void) {
  printf("Board started!\n");
  *(uint32_t*)(0x50000514) = 1 << 20;
  *(uint32_t*)(0x50000504) = 1 << 20;

  gpio_config(14, GPIO_INPUT);
  gpio_config(23, GPIO_INPUT);
  gpio_config(20, GPIO_OUTPUT);
  gpio_clear(20);

  // gpio_init();
  while (1) {
    if (button_pressed()) {
      printf("button pressed\n");
    }

    if (shout()) {
      printf("loud\n");
    }
  }
}


// App Timer example app
//
// Use the App Timer to blink LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

#include "button.h"
#include "flip.h"
#include "touch.h"
#include "twist.h"
#include "water.h"
#include "mic.h"
#include "utils.h"

bool button_pressed() {
  return (!gpio_read(14)) || (!gpio_read(23));
}

bool shout() {
  return microphone() > 15000;
}

bool flip() {
  lsm303agr_measurement_t m = lsm303agr_read_accelerometer();

  return m.z_axis > 0.8;
}

bool touching() {
  return touch() > 3.0;
}

bool twisting() {
  return twist();
}

bool watering() {
  return water() > 1.0;
}

typedef struct current_order_t {
  char* task;
  struct current_order_t *next;
} current_order_t;

int main(void) {
  printf("Board started!\n");
  *(uint32_t*)(0x50000514) = 1 << 20;
  *(uint32_t*)(0x50000504) = 1 << 20;

  gpio_config(14, GPIO_INPUT);
  gpio_config(23, GPIO_INPUT);
  gpio_config(20, GPIO_OUTPUT);
  gpio_clear(20);

  lsm303agr_init();
  srand(time(NULL));


  struct current_order_t* current;
  current = malloc(sizeof(struct current_order_t));
  struct current_order_t* head;
  head = malloc(sizeof(struct current_order_t));

  current->task = "new";
  current->next = NULL;

  head->task = "head";
  head->next = current;
  int success = 1;

  while(success == 1){
    struct current_order_t* current_temp;
    current_temp = head;
    while (current_temp->next->next != NULL){
      printf("Order");
      char* task = current_temp->next->task;

      if (task == "button") {
        bool pressed = false;

        while (!pressed) {
          pressed = button_pressed();
        }

        printf("button done!\n");
      } else if (task == "flip") {
        bool flipped = false;

        while (!flipped) {
          flipped = flip();
        }

        printf("flip done!\n");
      } else if (task == "mic") {
        bool shouted = false;

        while (!shouted) {
          shouted = shout();
        }

        shouted = false;
        printf("shout done!\n");
      } else if (task == "touch") {
        touching();

        printf("touch done!\n");
      } else if (task == "twist") {
        twisting();

        printf("twist done!\n");
      } else if (task == "water") {
        watering();

        printf("water done!\n");
      }

      current_temp = current_temp->next;
    }
    while (1) {
      printf("Random");
      char* task = choose();
      current->task = task;

      if (task == "button") {
        bool pressed = false;

        while (!pressed) {
          pressed = button_pressed();
        }

        printf("button done!\n");
      } else if (task == "flip") {
        bool flipped = false;

        while (!flipped) {
          flipped = flip();
        }

        printf("flip done!\n");
      } else if (task == "mic") {
        bool shouted = false;

        while (!shouted) {
          shouted = shout();
        }

        shouted = false;
        printf("shout done!\n");
      } else if (task == "touch") {
        touching();

        printf("touch done!\n");
      } else if (task == "twist") {
        twisting();

        printf("twist done!\n");
      } else if (task == "water") {
        watering();

        printf("water done!\n");
      }

      nrf_delay_ms(500);
      struct current_order_t* new;
      new = malloc(sizeof(struct current_order_t));
      new->task = "new";
      new->next = NULL;
      current->next = new;
      current = current->next;
    }
  }

  return 1;
}


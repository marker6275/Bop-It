// App Timer example app
//
// Use the App Timer to blink LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

#include "button.h"
#include "flip.h"
#include "touch.h"
#include "twist.h"
#include "water.h"
#include "light.h"
#include "utils.h"
#include "nrfx_timer.h"

nrfx_timer_t timer1 = NRFX_TIMER_INSTANCE(0);

void timer_interrupt(nrf_timer_event_t event_type, void *p_context) {
  printf("YOU LOSE!\n");
}

void timer_init(void) {
  nrfx_timer_config_t timer_config = NRFX_TIMER_DEFAULT_CONFIG;
  timer_config.frequency = NRF_TIMER_FREQ_1MHz;
  timer_config.bit_width = NRF_TIMER_BIT_WIDTH_32;
  nrfx_timer_init(&timer1, &timer_config, timer_interrupt);
  nrfx_timer_enable(&timer1);
}

int timer_done(float limit) {
  return nrfx_timer_capture(&timer1, NRF_TIMER_CC_CHANNEL0) > limit;
}

bool button_pressed() {
  return (!gpio_read(14)) || (!gpio_read(23));
}

bool lighting() {
  return light();
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

bool temp() {
  return temperature() > 3;
}

int main(void) {
  game_init();
  timer_init();
  opening_sequence();

  struct current_order_t* current;
  current = malloc(sizeof(struct current_order_t));
  
  struct current_order_t* head;
  head = malloc(sizeof(struct current_order_t));

  current->task = choose();
  current->next = NULL;

  head->task = "head";
  head->next = current;

  float limit = 6000000;
  int level = 1;
  
  while(1){
    struct current_order_t* current_temp;
    current_temp = head;
    while (current_temp->next->next != NULL){
      nrfx_timer_clear(&timer1);

      char* task = current_temp->next->task;
      print_do_sequence();
      int count = 0;
      if (task == "button") {
        bool pressed = false;

        while (!pressed) {
          pressed = button_pressed();
        }

        print_good();
      } else if (task == "flip") {
        
        bool flipped = false;

        while (!flipped) {
          flipped = flip();
        }

        print_good();
      } else if (task == "light") {
        lighting();

        print_good();
      } else if (task == "touch") {
        touching();

        print_good();
      } else if (task == "twist") {
        twisting();

        print_good();
      } else if (task == "water") {
        watering();

        print_good();
      }

        if (timer_done(limit)) {
          print_lose(level);
          return -1;
        }
      
      current_temp = current_temp->next;
      nrf_delay_ms(500);
    }

    // get new task to add
    char* task = choose();
    current->task = task;
    print_new_task(task, level);

    if (task == "button") {
      bool pressed = false;

      while (!pressed) {
        pressed = button_pressed();
      }

      print_done("button");
    } else if (task == "flip") {
      bool flipped = false;

      while (!flipped) {
        flipped = flip();
      }

      print_done("flip");
    } else if (task == "light") {
      lighting();

      print_done("light");
    } else if (task == "touch") {
      touching();

      print_done("touch");
    } else if (task == "twist") {
      twisting();

      print_done("twist");
    } else if (task == "water") {
      watering();

      print_done("water");
    }

    nrf_delay_ms(500);
    struct current_order_t* new;
    new = malloc(sizeof(struct current_order_t));
    new->task = "new";
    new->next = NULL;
    current->next = new;
    current = current->next;
    
    limit *= 0.95;
    level += 1;
  }

  return 1;
}


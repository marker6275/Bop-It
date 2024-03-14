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

const char* choices[6] = {"button", "flip", "touch", "twist", "water", "light"};
void generate(void) {
    srand(time(NULL));
}

char* choose(void) {
    int size = sizeof(choices) / sizeof(choices[0]);
    int index = random() % size;

    return choices[index];
}

void game_init(void) {
  // initialize buttons
  *(uint32_t*)(0x50000514) = 1 << 20;
  *(uint32_t*)(0x50000504) = 1 << 20;

  // gpio button configs
  gpio_config(14, GPIO_INPUT); // button A
  gpio_config(23, GPIO_INPUT); // button B
  gpio_config(20, GPIO_OUTPUT); // mic LED
  gpio_clear(20); // set to low

  generate();

  // initialize accelerometer
  lsm303agr_init();
}

void print_border(void) {
    printf("##########################################################\n");
}

void print_space(int size) {
    for (int i = 0; i < size; i++) {
        printf("\n");
    }
}

void opening_sequence(void) {
    print_space(25);
    
    print_border();
    print_space(12);
    printf("Game starting!\n");
    print_space(12);
    print_border();
    
    nrf_delay_ms(3000);

    print_space(12);
    printf("Ready?\n");
    print_space(12);
    print_border();

    nrf_delay_ms(2000);

    print_space(12);
    printf("Go!\n");
    print_space(12);
    print_border();

    nrf_delay_ms(1000);
}

void print_new_task(char* task, int level) {
    print_border();
    print_space(11);

    printf("LEVEL %d\n", level);
    printf("--------\n");
    printf("NEW TASK:               %s\n", task);

    print_space(11);
    print_border();
}

void print_do_sequence(void) {
    print_border();
    print_space(9);

    printf("DO YOU REMEMBER THE ORDER?\n");
    printf("  /\\  /\\\n");
    printf(" / o\\/o \\\n");
    printf("(    >   )\n");
    printf(" \\_^__^_/\n");
    printf("DON'T BE TOO SLOW!\n");

    print_space(10);
    print_border();
}

void print_done(char* task) {
    print_border();
    print_space(12);

    printf("%s done!\n", task);

    print_space(12);
    print_border();
}

void print_good(void) {
    print_border();
    print_space(12);

    printf("GOOD!\n");

    print_space(12);
    print_border();
}

void print_lose(int score) {
    print_border();
    print_space(9);

    printf("YOU WERE TOO SLOW!\n");
    printf("(______)\n");
    printf("| x  x |\n");
    printf(" \\    /\n");
    printf("  \\  /\n");
    printf("   (n)\n");
    printf("YOU LOSE!\n");
    printf("Score: %d\n", score - 1);

    print_space(8);
    print_border();
}
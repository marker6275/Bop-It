#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char* choices[] = {"button", "mic", "flip", "touch", "twist", "water"};

char* choose(void) {
    // srand(time(NULL));
    int size = sizeof(choices) / sizeof(choices[0]);
    int index = rand() % size;

    printf("%s\n", choices[index]);

    return choices[index];
}
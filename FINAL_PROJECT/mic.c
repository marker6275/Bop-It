// Record and Play App
//
// Record audio using the microphone, ADC, and timer
// Play back that audio using the speaker and PWM

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

// Analog input
#define ANALOG_MIC_IN NRF_SAADC_INPUT_AIN3

// ADC channel configurations
#define ADC_MIC_CHANNEL 0
#define ADC_MAX_COUNTS (16384)

// Timer configuration
#define TIMER_TICKS (16000000/SAMPLING_FREQUENCY)

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Sample data configurations
// Note: this is a 62.5 kB buffer (almost half of RAM)
#define SAMPLING_FREQUENCY 16000 // 16 kHz sampling rate
#define BUFFER_SIZE 16000 // two seconds worth of data
uint16_t samples[BUFFER_SIZE] = {0}; // stores ADC samples and PWM duty cycle values
volatile bool samples_complete = false; // flag for blocking while sampling
uint32_t avg = 0;

void TIMER4_IRQHandler(void) {
  // Needs to be quick! No printf here!!

  // Clear the event
  NRF_TIMER4->EVENTS_COMPARE[0] = 0;

  // Set the next timer based on the previous (avoid drift)
  NRF_TIMER4->CC[0] = NRF_TIMER4->CC[0] + TIMER_TICKS;

  // Sample the ADC (captures data over DMA, non-blocking)
  nrfx_saadc_sample();
}

static void saadc_event_callback(nrfx_saadc_evt_t const* event) {
  if (event->type == NRFX_SAADC_EVT_DONE) {

    // Done sampling, stop the timer
    NRF_TIMER4->CC[0] = 0;
    NRF_TIMER4->TASKS_STOP = 1;

    // determine the average of the samples
    uint32_t average = 0;
    for (int i=0; i<BUFFER_SIZE; i++) {
      average += (uint16_t) samples[i];
    }
    average = average / BUFFER_SIZE;

    // scale each sample based on the average value and recenter around 50%

    for (int i=0; i<BUFFER_SIZE; i++) {
      // scaling determined experimentally
      samples[i] = (((int32_t)samples[i] - average) * 10) + (ADC_MAX_COUNTS/2);
      avg += samples[i];
    }

    avg /= BUFFER_SIZE;
    // Signal completion
    samples_complete = true;

  } else {
    printf("Got some other SAADC event?!\n");
  }
}

static void gpio_init(void) {
  // Initialize pins
  // Microphone pin MUST be high drive
  nrf_gpio_pin_dir_set(LED_MIC, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_cfg(LED_MIC, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0H1, NRF_GPIO_PIN_NOSENSE);

  // Enable microphone
  nrf_gpio_pin_set(LED_MIC);
}

static void adc_init(void) {
  // Initialize the SAADC
  nrfx_saadc_config_t saadc_config = {
    .resolution = NRF_SAADC_RESOLUTION_14BIT,
    .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
    .interrupt_priority = 1, // should be higher than timer
    .low_power_mode = false,
  };
  nrfx_saadc_init(&saadc_config, saadc_event_callback);
  // Initialize the microphone ADC channel
  // It's a small signal we're sampling quickly, so max out gain and minimize
  //    acquisition time
  nrf_saadc_channel_config_t mic_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_MIC_IN);
  mic_channel_config.gain = NRF_SAADC_GAIN4;
  mic_channel_config.acq_time = NRF_SAADC_ACQTIME_3US;
  nrfx_saadc_channel_init(ADC_MIC_CHANNEL, &mic_channel_config);
}

static void timer_init(void) {
   // Set to 32 bit timer
  NRF_TIMER4->BITMODE = 3;

  // Set to 16 MHz clock
  NRF_TIMER4->PRESCALER = 0;

  // Enable interrupts (not the 0th bit!)
  NRF_TIMER4->CC[0] = 0;
  NRF_TIMER4->INTENSET = 1 << TIMER_INTENSET_COMPARE0_Pos;

  // Enable interrupts in the NVIC
  NVIC_ClearPendingIRQ(TIMER4_IRQn);
  NVIC_SetPriority(TIMER4_IRQn, 7); // lowest priority
  NVIC_EnableIRQ(TIMER4_IRQn);

}

static void sample_microphone(void) {
  // save a flag for blocking on later
  samples_complete = false;

  // prepare the sample the ADC
  nrfx_saadc_buffer_convert((int16_t*)samples, BUFFER_SIZE);
//   printf("%hn\n", samples);
  // clear and start timer, set interrupt
  NRF_TIMER4->TASKS_CLEAR = 1;
  NRF_TIMER4->TASKS_START = 1;
  NRF_TIMER4->CC[0] = TIMER_TICKS; // 16 kHZ
}

static void pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 16 MHz
  // Set a countertop value based on sampling frequency and repetitions
  // TODO
  nrfx_pwm_config_t config;
  config.output_pins[0] = SPEAKER_OUT;
  config.output_pins[1] = NRFX_PWM_PIN_NOT_USED;
  config.output_pins[2] = NRFX_PWM_PIN_NOT_USED;
  config.output_pins[3] = NRFX_PWM_PIN_NOT_USED;
  config.base_clock = NRF_PWM_CLK_16MHz;
  config.count_mode = NRF_PWM_MODE_UP;
  config.top_value = ((16000000 / 4) / SAMPLING_FREQUENCY);
  config.load_mode = NRF_PWM_LOAD_COMMON;
  config.step_mode = NRF_PWM_STEP_AUTO;

  nrfx_pwm_init(&PWM_INST, &config, NULL);
}

uint32_t microphone(void) {
  // Initialize GPIO
  gpio_init();

  // Initialize ADC
  adc_init();

  // Initialize timer
  timer_init();

  // Initialize the PWM
  pwm_init();

  // Sample audio from the microphone
  sample_microphone();

  while (!samples_complete) {
    nrf_delay_ms(100);
  }

  return avg;
}


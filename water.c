// Breadboard example app
//
// Read from multiple analog sensors and control an RGB LED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

#define ANALOG_WATER_IN NRF_SAADC_INPUT_AIN4

#define ADC_WATER_CHANNEL 4

static void adc_init(void);
static float adc_sample_blocking(uint8_t channel);


static void saadc_event_callback(nrfx_saadc_evt_t const* _unused) {
  // don't care about saadc events
  // ignore this function
}

static void adc_init(void) {
  // Initialize the SAADC
  nrfx_saadc_config_t saadc_config = {
    .resolution = NRF_SAADC_RESOLUTION_12BIT,
    .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
    .interrupt_priority = 4,
    .low_power_mode = false,
  };
  ret_code_t error_code = nrfx_saadc_init(&saadc_config, saadc_event_callback);

  nrf_saadc_channel_config_t light_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_WATER_IN);
  error_code = nrfx_saadc_channel_init(ADC_WATER_CHANNEL, &light_channel_config);
}

static float adc_sample_blocking(uint8_t channel) {
  int16_t adc_counts = 0;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &adc_counts);

  float voltage = (3.6 * adc_counts) / 4096;

  return voltage;
}

float water(void) {  
  adc_init();

  bool touched = false;

  while (!touched) {
    touched = adc_sample_blocking(4) > 1;
  }

  return adc_sample_blocking(4);
}


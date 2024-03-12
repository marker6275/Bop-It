// LSM303AGR driver for Microbit_v2
//
// Initializes sensor and communicates over I2C
// Capable of reading temperature, acceleration, and magnetic field strength

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "flip.h"
#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

static int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
  
  int8_t rx_buf = 0;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
  };
  
  ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
  if (result != NRF_SUCCESS) {
    // Likely error codes:
    //  NRF_ERROR_INTERNAL            (0x0003) - something is wrong with the driver itself
    //  NRF_ERROR_INVALID_ADDR        (0x0010) - buffer passed was in Flash instead of RAM
    //  NRF_ERROR_BUSY                (0x0011) - driver was busy with another transfer still
    //  NRF_ERROR_DRV_TWI_ERR_OVERRUN (0x8200) - data was overwritten during the transaction
    //  NRF_ERROR_DRV_TWI_ERR_ANACK   (0x8201) - i2c device did not acknowledge its address
    //  NRF_ERROR_DRV_TWI_ERR_DNACK   (0x8202) - i2c device did not acknowledge a data byte
    printf("READ I2C transaction failed! Error: %lX\n", result);
  }

  return rx_buf;
}

static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
  uint8_t byte_array[2] = {reg_addr, data};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, byte_array, 2, NRF_TWI_MNGR_NO_STOP)
  };
  ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  if (result != NRF_SUCCESS) {
    // Likely error codes:
    //  NRF_ERROR_INTERNAL            (0x0003) - something is wrong with the driver itself
    //  NRF_ERROR_INVALID_ADDR        (0x0010) - buffer passed was in Flash instead of RAM
    //  NRF_ERROR_BUSY                (0x0011) - driver was busy with another transfer still
    //  NRF_ERROR_DRV_TWI_ERR_OVERRUN (0x8200) - data was overwritten during the transaction
    //  NRF_ERROR_DRV_TWI_ERR_ANACK   (0x8201) - i2c device did not acknowledge its address
    //  NRF_ERROR_DRV_TWI_ERR_DNACK   (0x8202) - i2c device did not acknowledge a data byte
    printf("WRITE I2C transaction failed! Error: %lX\n", result);
  }
}

void lsm303agr_init(void) {
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = I2C_INTERNAL_SCL;
  i2c_config.sda = I2C_INTERNAL_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);

  i2c_manager = &twi_mngr_instance;

  // ---Initialize Accelerometer---

  // Reboot acclerometer
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG5_A, 0x80);
  nrf_delay_ms(100); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG4_A, 0x80);

  // Configure accelerometer at 100Hz, normal mode (10-bit)
  // Enable x, y and z axes
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG1_A, 0x57);
}

lsm303agr_measurement_t lsm303agr_read_accelerometer(void) {
  int8_t xl = (int8_t) i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_X_L_A);
  int8_t xh = (int8_t) i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_X_H_A);
  int8_t yl = (int8_t) i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Y_L_A);
  int8_t yh = (int8_t) i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Y_H_A);
  int8_t zl = (int8_t) i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Z_L_A);
  int8_t zh = (int8_t) i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Z_H_A);

  int16_t x = ((xh << 8) | xl) >> 6;
  int16_t y = ((yh << 8) | yl) >> 6;
  int16_t z = ((zh << 8) | zl) >> 6;

  lsm303agr_measurement_t measurement = {(float) (x * 0.0039), (float) (y * 0.0039), (float) (z * 0.0039)};

  return measurement;
}
// LSM303AGR accelerometer and magnetometer

#pragma once

#include "nrf_twi_mngr.h"

static const uint8_t LSM303AGR_ACC_ADDRESS = 0x19;

typedef struct {
  float x_axis;
  float y_axis;
  float z_axis;
} lsm303agr_measurement_t;

typedef enum {
  CTRL_REG1_A = 0x20,
  CTRL_REG4_A = 0x23,
  CTRL_REG5_A = 0x24,
  OUT_X_L_A = 0x28,
  OUT_X_H_A = 0x29,
  OUT_Y_L_A = 0x2A,
  OUT_Y_H_A = 0x2B,
  OUT_Z_L_A = 0x2C,
  OUT_Z_H_A = 0x2D,
} lsm303agr_acc_reg_t;

void lsm303agr_init(void);

lsm303agr_measurement_t lsm303agr_read_accelerometer(void);
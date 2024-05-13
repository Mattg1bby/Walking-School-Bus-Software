#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "ets_sys.h"
#include "osapi.h"
#include "driver/i2c_master.h"

#include "user_interface.h"

typedef struct {
    sint16_t x;
    sint16_t y;
    sint16_t z;
} AccelerometerData;

static const uint8_t MMA8451QAddress = 0x1D;

void initAccelorometer();

void getAccelorometerData(AccelerometerData *data);

sint16_t convert (uint8_t MSB, uint8_t LSB);

void print_float (float value);

#endif 
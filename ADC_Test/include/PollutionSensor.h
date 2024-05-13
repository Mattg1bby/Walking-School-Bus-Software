#ifndef POLLUTIONSENSOR_H
#define POLLUTIONSENSOR_H

#include "ets_sys.h"
#include "osapi.h"
#include "driver/i2c_master.h"

#include "user_interface.h"

static const  uint8_t ZMOD4510Address = 0x33; //Pollution Sensor

void initPolutionSensor();


#endif 
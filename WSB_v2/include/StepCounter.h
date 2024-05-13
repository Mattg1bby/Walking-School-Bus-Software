#ifndef STEPCOUNTER_H
#define STEPCOUNTER_H

#include "ets_sys.h"
#include "osapi.h"
#include "driver/i2c_master.h"

#include "user_interface.h"

int step_count;

void stepCounter(sint16_t X, sint16_t Y, sint16_t Z);

#endif 
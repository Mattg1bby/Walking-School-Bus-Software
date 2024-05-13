#ifndef I2C_FUNC_H
#define I2C_FUNC_H

#include "ets_sys.h"
#include "osapi.h"
#include "driver/i2c_master.h"

#include "user_interface.h"

bool ack;

bool beginTransmission(uint8_t i2c_addr);
bool write(uint8_t i2c_data);
bool requestFrom(uint8_t reg_addr, uint8_t i2c_addr);
bool multipleByteRead(uint8_t num_bytes, int8_t* data);
bool singleByteRead(uint8_t *data);
bool singleByteReadEnd(uint8_t *data);
bool checkAck(bool ack);
void endTransmission();

#endif 


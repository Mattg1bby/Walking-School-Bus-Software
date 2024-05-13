#include "I2C_Func.h"


bool beginTransmission(uint8_t i2c_addr) {
    ack = 0;
    uint8_t i2c_addr_write = (i2c_addr << 1);
    i2c_master_start();
    i2c_master_writeByte(i2c_addr_write);
    ack = i2c_master_checkAck();
    return ack;
}

bool write(uint8_t i2c_data) {
    ack = 0;
    i2c_master_writeByte(i2c_data);
    ack = i2c_master_checkAck();
    return ack;
}

bool requestFrom(uint8_t reg_addr, uint8_t i2c_addr) {
    uint8_t i2c_addr_read = (i2c_addr << 1) + 1;
    if (checkAck(ack) == 1) {
        write(reg_addr);
    }
    if (checkAck(ack) == 1) {
        i2c_master_start();
        i2c_master_writeByte(i2c_addr_read);
    }
    ack = i2c_master_checkAck();
    return ack;
    os_delay_us(100);
}

bool multipleByteRead(uint8_t num_bytes, int8_t* data) {
    if (num_bytes < 1 || data == NULL)
        return false;

    for (int i = 0; i < num_bytes; i++) {
        data[i] = i2c_master_readByte();
        i2c_master_send_ack();
    }

    i2c_master_send_nack();

    return true;
}

bool singleByteRead(uint8_t *data) {
    if (data == NULL)
        return false;

    *data = i2c_master_readByte();
    i2c_master_send_ack();

    return true;
}

bool singleByteReadEnd(uint8_t *data) {
    if (data == NULL)
        return false;

    *data = i2c_master_readByte();
    i2c_master_send_nack();

    return true;
}

bool checkAck(bool ack) {
    if (ack == 1)
        return true;
    else {
        endTransmission();
        return false;
    }
}

void endTransmission() {
    i2c_master_stop();
}


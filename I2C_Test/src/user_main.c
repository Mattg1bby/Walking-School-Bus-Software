/* Matthew Fitzgibbons 01/03/2024
 *
 * Code used for developing i2c function with the MMA8451Q Accelorometre being used as the slave device to
 * test whether operational or not
 *
 * ------------------------NOTES--------------------------------
 *
 * Defined in i2c_master.h: SDA is GPIO2, and SCL is GPIO14
 *  (these are not hardware limitations, however, as the ESP8266
 *  implements i2c in software, so any GPIO pins should work)
 */

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "driver/i2c_master.h"
#include "ets_sys.h"
//#include "espconn.h"

#include "user_interface.h"

#define procTaskQueueLen 1
#define userProcTaskPrio 0
os_event_t procTaskQueue[procTaskQueueLen];
void loop(os_event_t* events);

//--------------------------------------------------------------
// Variables:
//--------------------------------------------------------------
const uint8_t MMA8451QAddress = 0x1D; //Accelerometer
const uint8_t ZMOD4510Address = 0x33; //Pollution Sensor
bool ack = 0;
int8_t test_byte = 0;
int8_t test_array[8] = {0,0,0,0,0,0,0,0};

//--------------------------------------------------------------
// Predefinitions:
//--------------------------------------------------------------
bool beginTransmission(uint8_t i2c_addr);
bool requestFrom(uint8_t reg_addr, uint8_t i2c_addr);
bool multipleByteRead(uint8_t num_bytes, int8_t* data);
bool singleByteRead(uint8_t *data);
bool singleByteReadEnd(uint8_t *data);
bool write(uint8_t i2c_data);
bool checkAck(bool ack);
void endTransmission();
void printArray(uint8_t num_bytes, int8_t* data);
void delay_s (uint8_t sec);
sint16_t convert (uint8_t MSB, uint8_t LSB);
void print_float (float value);

//--------------------------------------------------------------
// Functions for reading and controlling
//--------------------------------------------------------------
void initAccelorometer();
void initPolutionSensor();

void getAccelorometerData();

uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}

//--------------------------------------------------------------
// Main
//--------------------------------------------------------------
void ICACHE_FLASH_ATTR user_init()
{
    uart_init(115200, 115200);       // Setting Uart Speed
    os_printf("\n");
    os_delay_us(10000); //10ms
    gpio_init();
    os_delay_us(10000); //10ms
    i2c_master_gpio_init();
    os_delay_us(10000); //10ms
    i2c_master_init();
    os_delay_us(10000); //10ms

    initAccelorometer();
    os_delay_us(10000); //10ms
    //initPolutionSensor();
    os_delay_us(10000); //10ms

    system_os_task(loop, userProcTaskPrio, procTaskQueue, procTaskQueueLen);
    system_os_post(userProcTaskPrio, 0, 0); // */
    os_printf("Init done\r\n");
}

//--------------------------------------------------------------
// Loop
//--------------------------------------------------------------
void loop(os_event_t* events)
{
    //os_printf("------------------------Start Loop---------------------\r\n");
    getAccelorometerData();
    os_delay_us(100); //0.1ms
    system_os_post(userProcTaskPrio, 0, 0);
    //os_printf("-------------------------End Loop--------------------\r\n");

}

//--------------------------------------------------------------
// Functions Definitions for controlling functions
//--------------------------------------------------------------
void initPolutionSensor(){  
    /*//Write to a writable adress
        beginTransmission(ZMOD4510Address);
        if(checkAck(ack) == 1){
        write(0x88); //Register Address   
        }
        if(checkAck(ack) == 1){
        write(0x46); //Data - Reset Device - 0100 0000   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);//*/

    //Read address just written to
        os_printf("---- Read data Start ----\r\n");
        beginTransmission(ZMOD4510Address);
        requestFrom(0x88,ZMOD4510Address);//Request to Read
        singleByteReadEnd(&test_byte); //Read Address
        endTransmission(); //End transmission
        os_printf("Read Register Polution Sensor 0x88: 0x%X\r\n", test_byte);
        os_printf("---- Read data End----\r\n");
        os_delay_us(100); //0.1ms*/
}

void initAccelorometer(){  
    //Reset MMA8451QAddress
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x2B); //Register Address   
        }
        if(checkAck(ack) == 1){
        write(0x40); //Data - Reset Device - 0100 0000   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(1000);

    //Set to Standby Mode
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x2A); //Register Address   
        }
        if(checkAck(ack) == 1){
        write(0x00); //Data - Set to Standby mode - 00000000   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);
    
    //Sets the F setup register 
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x09); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x00); //Data - FIFO Disabled - 00000000   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);

    //Test Writing to register 0x18
       /*beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x18); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x56);
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);*/

    //Sets the dynamic range 
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x0E); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x00); //Data - Set dynamic range 2g - 00000000   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);

    //Set to Active Mode
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x2A); //Register Address   
        }
        if(checkAck(ack) == 1){
        write(0x01); //Data - Set to Active mode and Fast Read mode (1 Byte) - 00000001   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);

        //Test Reading WHO_AM_I register
        os_printf("---- Read data Start ----\r\n");
        beginTransmission(MMA8451QAddress);
        requestFrom(0x18,MMA8451QAddress);//Request to Read - Bug in this functions
        singleByteReadEnd(&test_byte); //Read Address
        endTransmission(); //End transmission
        os_printf("Read data Register 0x18: 0x%X\r\n", test_byte);
        os_printf("---- Read data End----\r\n");
        os_delay_us(100); //0.1ms*/
}

void getAccelorometerData()
{
    bool XYZDR_Flag = 0;
    int8_t F_Status = 0;

    int8_t X_out_MSB = 0;
    int8_t X_out_LSB = 0;
    sint16_t X_out = 0;

    int8_t Y_out_MSB = 0;
    int8_t Y_out_LSB = 0;
    sint16_t Y_out = 0;

    int8_t Z_out_MSB = 0;
    int8_t Z_out_LSB = 0;
    sint16_t Z_out = 0;

    //os_printf("---- Get Accelorometer Start ----\r\n");

    //Read F
    beginTransmission(MMA8451QAddress);
    requestFrom(0x00,MMA8451QAddress);//Request to Read F Status
    singleByteReadEnd(&F_Status); //Read Address
    endTransmission(); //End transmission
    os_printf("F status: 0x%X\r\n", F_Status);
    os_delay_us(100); //0.1ms

    XYZDR_Flag = (F_Status & 0x08)>>3;
    os_printf("XYZDR_Flag: %d\r\n", XYZDR_Flag);

    if(XYZDR_Flag == 1){
       
        beginTransmission(MMA8451QAddress);
        requestFrom(0x01,MMA8451QAddress);//Request to OUT_X_MSB
        singleByteRead(&X_out_MSB); //Auto Increments through the next registers
        singleByteRead(&X_out_LSB);
        singleByteRead(&Y_out_MSB);
        singleByteRead(&Y_out_LSB);
        singleByteRead(&Z_out_MSB);
        singleByteReadEnd(&Z_out_LSB);
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms
       
       
       
        /*//Get X Data
        beginTransmission(MMA8451QAddress);
        requestFrom(0x01,MMA8451QAddress);//Request to OUT_X_MSB
        singleByteReadEnd(&X_out_MSB); //Read Address
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms
        requestFrom(0x02,MMA8451QAddress);//Request to OUT_X_LSB
        singleByteReadEnd(&X_out_LSB); //Read Address
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms

        //Get Y Data
        beginTransmission(MMA8451QAddress);
        requestFrom(0x03,MMA8451QAddress);//Request to OUT_X_MSB
        singleByteReadEnd(&Y_out_MSB); //Read Address
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms
        beginTransmission(MMA8451QAddress);
        requestFrom(0x04,MMA8451QAddress);//Request to OUT_X_LSB
        singleByteReadEnd(&Y_out_LSB); //Read Address
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms

        //Get Z Data
        beginTransmission(MMA8451QAddress);
        requestFrom(0x05,MMA8451QAddress);//Request to OUT_X_MSB
        singleByteReadEnd(&Z_out_MSB); //Read Address
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms
        beginTransmission(MMA8451QAddress);
        requestFrom(0x06,MMA8451QAddress);//Request to OUT_X_LSB
        singleByteReadEnd(&Z_out_LSB); //Read Address
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms*/

        X_out = convert(X_out_MSB, X_out_LSB);
        Y_out = convert(Y_out_MSB, Y_out_LSB);
        Z_out = convert(Z_out_MSB, Z_out_LSB);

        /*os_printf("OUT_X_MSB: 0x%X\t", X_out_MSB);
        os_printf("OUT_X_LSB: 0x%X\n", X_out_LSB);

        os_printf("OUT_Y_MSB: 0x%X\t", Y_out_MSB);
        os_printf("OUT_Y_LSB: 0x%X\n", Y_out_LSB);

        os_printf("OUT_Z_MSB: 0x%X\t", Z_out_MSB);
        os_printf("OUT_Z_LSB: 0x%X\n", Z_out_LSB);//*/

        /*if( (0.120 < X_out/82.0 < 0.122) || (0.120 < Y_out/82.0 < 0.
        2) || (0.120 < Z_out/82.0 < 0.122)){
        print_float(X_out/82.0); os_printf(", "); print_float(Y_out/82.0); os_printf(", "); print_float(Z_out/82.0); os_printf(", ");
        }*/

        //os_printf("OUT_X: "); print_float(X_out/82.0); os_printf("\t");
        //os_printf("OUT_Y: "); print_float(Y_out/82.0); os_printf("\t");
        //os_printf("OUT_Z: "); print_float(Z_out/82.0); os_printf("\n");
    
    }
    else{
        os_printf("No new data\r\n");
    }

//os_printf("---- Get Accelorometer End ----\r\n");
}


//--------------------------------------------------------------
// Definition for sub-functions
//--------------------------------------------------------------
bool beginTransmission(uint8_t i2c_addr){
    ack = 0;
    uint8_t i2c_addr_write = (i2c_addr << 1);
    os_printf("i2c addr write: 0x%X\r\n",i2c_addr_write );

    i2c_master_start();
    i2c_master_writeByte(i2c_addr_write);
    
    ack = i2c_master_checkAck();

    return ack;
}

bool write(uint8_t i2c_data){
    ack = 0;
    os_printf("Write: 0x%X\r\n", i2c_data);
    i2c_master_writeByte(i2c_data);
 
    ack = i2c_master_checkAck();
    //os_printf("Write ACK: %X\r\n", ack);
    return ack;
}

bool requestFrom(uint8_t reg_addr, uint8_t i2c_addr){
    uint8_t i2c_addr_read = (i2c_addr << 1) + 1;
    
    if(checkAck(ack) == 1){
    write(reg_addr);
    }

    if(checkAck(ack) == 1){
    uint8_t i2c_addr_read = (i2c_addr << 1) + 1;
    
    i2c_master_start();
    os_printf("Read Device Address: 0x%X\r\n",   i2c_addr_read);
    i2c_master_writeByte(i2c_addr_read);
    }
    
    ack = i2c_master_checkAck();
    os_printf("Read device ACK: %d\r\n", ack);
    return ack;
    os_delay_us(100);
}

bool multipleByteRead(uint8_t num_bytes, int8_t* data){
    if (num_bytes < 1 || data == NULL) return false;

    for(int i = 0; i < num_bytes; i++)
    {
        data[i] = i2c_master_readByte();
        i2c_master_send_ack();
    }
    
    // nack the final packet so that the slave releases SDA
    //data[num_bytes - 1] = i2c_master_readByte();
    i2c_master_send_nack();

    return true;
}

bool singleByteRead(uint8_t *data){
    if (data == NULL) return false;
    os_printf("Byte read \n");
    *data = i2c_master_readByte();

    i2c_master_send_ack();

    return true;
}

bool singleByteReadEnd(uint8_t *data){
    if (data == NULL) return false;
    os_printf("Final Byte read \n");
    *data = i2c_master_readByte();

    i2c_master_send_nack();

    return true;
}

bool checkAck(bool ack){
    if(ack == 1){
        os_printf("Ack Recieved\r\n");
        return true;
    }
    else{
        os_printf("Error: No Ack Recieved\r\n");
        endTransmission();
        return false;
    }
}

void endTransmission(){
    //os_printf("Stop \r\n");
    i2c_master_stop();
}

void printArray(uint8_t num_bytes, int8_t* data){
    i2c_master_stop();

    for(int i=0; i<num_bytes; i++){
        os_printf("Read data Print: 0x%X\r\n", data[i]);
     }
}

void delay_s (uint8_t sec){

    for(int i=0; i < 20*sec; i++){
        os_delay_us(50000); //50 ms

    }
}

sint16_t convert (uint8_t MSB, uint8_t LSB){
    
    uint16_t newMSB = MSB;
    uint16 newLSB = LSB;
    newMSB = newMSB << 8;
    uint16_t data = (newMSB + newLSB) >> 2;

    if(MSB & 0x80){
        return data + 0xC000;
     }
    else{
        return data;
    }
   
}

void print_float (float value){
    int16_t wholePart = (int)value;
    int16_t decimalPart = (int)((value-wholePart)*10000);

    if (value < 0){
        decimalPart = decimalPart * -1;
    }

    os_printf("%d.", wholePart);
    os_printf("%d", decimalPart);
}
/* Matthew Fitzgibbons 01/03/2024
 *
 *This is the code which was used to develop and test the implementation of the step counter on ESPWROOM02
 *
 * ------------------------NOTES--------------------------------
 *
 * Defined in i2c_master.h: SDA is GPIO2, and SCL is GPIO14
 *   (these are not hardware limitations, however, as the ESP8266
 *   implements i2c in software, so any GPIO pins should work)
 */

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "driver/i2c_master.h"

#include "I2C_Func.h"
#include "Accelerometer.h"
#include "Interrupts.h"

#include "user_interface.h"

#define procTaskQueueLen 1
#define userProcTaskPrio 0
os_event_t procTaskQueue[procTaskQueueLen];
void loop(os_event_t* events);

//--------------------------------------------------------------
// Variables:
//--------------------------------------------------------------
int8_t test_byte = 0;
int step_count = 0;
int16_t X = 0;
//--------------------------------------------------------------
// Predefinitions:
//--------------------------------------------------------------
void printArray(uint8_t num_bytes, int8_t* data);
void delay_s (uint8_t sec);
void handleAccelerometerInterrupt(void *arg);

//--------------------------------------------------------------
// Functions for reading and controlling
//--------------------------------------------------------------
void configAccelorometer();
void stepCounter(sint16_t X, sint16_t Y, sint16_t Z);

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
    os_delay_us(1000); //1ms
    gpio_init();
    os_delay_us(1000); //1ms
    i2c_master_gpio_init();
    os_delay_us(1000); //1ms
    i2c_master_init();
    os_delay_us(1000); //1ms
    //initInterrupt();
    os_delay_us(10000); //1ms
    initAccelorometer();
    os_delay_us(10000); //1ms
    //configAccelorometer();
    os_delay_us(10000); //10ms

    /*ETS_GPIO_INTR_DISABLE();
    ETS_GPIO_INTR_ATTACH(&handleAccelerometerInterrupt, NULL);
    gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
    ETS_GPIO_INTR_ENABLE();//*/

    system_os_task(loop, userProcTaskPrio, procTaskQueue, procTaskQueueLen);
    system_os_post(userProcTaskPrio, 0, 0); // */
    os_printf("Init done\r\n");
}

//--------------------------------------------------------------
// Functions Definitions for controlling functions
//--------------------------------------------------------------
void loop(os_event_t* events)
{
    //os_printf("------------------------Start Loop---------------------\r\n");
    AccelerometerData accelData;
    getAccelorometerData(&accelData);
    os_delay_us(100);
    stepCounter(accelData.x, accelData.y, accelData.z);
    os_delay_us(100); //0.1ms
    system_os_post(userProcTaskPrio, 0, 0);
    //os_printf("-------------------------End Loop--------------------\r\n");
    //os_delay_us(10000); //10ms
}

void configAccelorometer(){  
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
        os_delay_us(100);

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
    
    //Set the Threshold 
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x1F); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x08); //Data - 00001000   
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);

    //Sets the debounce counter
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x20); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x05); //Data - 00000101  
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);

    //Enable Transient Detection
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x2D); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x20); //Data - 00100000  
        }
        if(checkAck(ack) == 1){
        endTransmission();  
        }
        os_delay_us(100);

    //Route Transient Interrupt INT1 pin
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == 1){
        write(0x2E); //Register Address
        }
        if(checkAck(ack) == 1){
        write(0x20); //Data - 00100000  
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
        requestFrom(0x0D,MMA8451QAddress);//Request to Read - Bug in this functions
        singleByteReadEnd(&test_byte); //Read Address
        endTransmission(); //End transmission
        os_printf("Read data WHO_AM_I 0x0D: 0x%X\r\n", test_byte);
        os_printf("---- Read data End----\r\n");
        os_delay_us(100); //0.1ms*/
}


//--------------------------------------------------------------
// Definition for sub-functions
//--------------------------------------------------------------
void delay_s (uint8_t sec){

    for(int i=0; i < 20*sec; i++){
        os_delay_us(50000); //50 ms

    }
}

void stepCounter(sint16_t X, sint16_t Y, sint16_t Z){
    //os_printf("%d,%d,%d,",  X,Y,Z);
    //os_printf("%d\t%d\t%d\n",  X,Y,Z);
//Protection limiters to stop a step being counted when its just an orientation change
 if((-1500 < Y && Y < 1500) && (-1500 < Z && Z < 1500)){
    
    //If X facing Up
    if((2000 < X && X < 6000)){
        if (X > 5000 ){
            step_count++;
            //debounce timer 250ms
            os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);
            os_delay_us(50000);os_delay_us(50000); 
            os_printf("Steps : %d \n", step_count);
        }
    }
    //If X Facing Down
    else if (-6000 < X && X < -2000){
        if (X < -4800 ){
            step_count++;
            //debounce timer 250ms
            os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);
            os_delay_us(50000);os_delay_us(50000); 
            os_printf("Steps : %d \n", step_count);
        }
    }

 }//*/
}

void handleAccelerometerInterrupt(void *arg) {
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    inter_count++;
    os_printf("Interrupt Triggered Called\n");    // Test If Trigger is called
    os_delay_us(15000); //15ms
  

    if (gpio_status & BIT(4)) {
        os_printf("Inter Count %d\n", inter_count);

        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(4));
        gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
    }

}


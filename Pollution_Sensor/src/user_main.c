/* Matthew Fitzgibbons 01/03/2024
 *
 * Example to implement code for the pollution sensor, This code is incomplete as porting of the code
 * for the ZMOD4510 firmware has begun but is not finished.
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
#include "user_interface.h"

#include "I2C_Func.h"
#include "Accelerometer.h"
#include "PollutionSensor.h"
#include "Interrupts.h"

#include "zmod4xxx.h"

/*#include "zmod4510_config_oaq2.h"
#include "zmod4xxx.h"
#include "zmod4xxx_cleaning.h"
#include "zmod4xxx_hal.h"
#include "oaq_2nd_gen.h"*/

#define procTaskQueueLen 1
#define userProcTaskPrio 0
os_event_t procTaskQueue[procTaskQueueLen];
void loop(os_event_t* events);

//--------------------------------------------------------------
// Variables:
//--------------------------------------------------------------


//--------------------------------------------------------------
// Predefinitions:
//--------------------------------------------------------------
void delay_s (uint8_t sec);

//--------------------------------------------------------------
// Functions for reading and controlling
//--------------------------------------------------------------

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
    os_delay_us(10000); //10ms
    gpio_init();
    os_delay_us(10000); //10ms
    i2c_master_gpio_init();
    os_delay_us(10000); //10ms
    i2c_master_init();
    os_delay_us(10000); //10ms
    initAccelorometer();
    os_delay_us(10000); //10ms
    initPolutionSensor();
    os_delay_us(10000); //10ms


    /*system_os_task(loop, userProcTaskPrio, procTaskQueue, procTaskQueueLen);
    system_os_post(userProcTaskPrio, 0, 0); // */
    os_printf("Init done\r\n");
}

//--------------------------------------------------------------
// Loop
//--------------------------------------------------------------
void loop(os_event_t* events)
{
    //os_printf("------------------------Start Loop---------------------\r\n");
    os_delay_us(100); //0.1ms
    system_os_post(userProcTaskPrio, 0, 0);
    //os_printf("-------------------------End Loop--------------------\r\n");
    os_delay_us(50000);
}

//--------------------------------------------------------------
// Functions Definitions for controlling functions
//--------------------------------------------------------------

//--------------------------------------------------------------
// Definition for sub-functions
//--------------------------------------------------------------
void delay_s (uint8_t sec){
    for(int i=0; i < 20*sec; i++){
        os_delay_us(50000); //50 ms
    }
}

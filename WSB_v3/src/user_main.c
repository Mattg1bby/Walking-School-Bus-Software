/* Matthew Fitzgibbons 10/02/2024
 *
 * WSB V3, contains the implmentstion of the buzzer code along with an updated RSSI function which has a rolling average, This is the most upto date version of the code
 *
 * ------------------------NOTES--------------------------------
 *
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
#include "StepCounter.h"
#include "Wifi.h"
#include "Buzzer.h"

#define procTaskQueueLen 1
#define userProcTaskPrio 0
os_event_t procTaskQueue[procTaskQueueLen];
void loop(os_event_t* events);//*/

//static os_timer_t ptimer;

uint32_t ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
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
// Variables:
//--------------------------------------------------------------
int inter_count = 0;
int step_count = 0;
uint16_t adc_val = 0;
float adc_calib_val = 0;
AccelerometerData accelData;

//--------------------------------------------------------------
// Declarations:
//--------------------------------------------------------------
void input_gpio(void);
void batteryADC(void);
void buzzernote(int freq);

void delay_ms(int ms);
void delay_s(int s);

//void loop(void *arg);

//--------------------------------------------------------------
// Main:
//--------------------------------------------------------------
void ICACHE_FLASH_ATTR user_init(void)
{
        uart_init(115200, 115200);       // Setting Uart Speed
        os_printf("\n");
        os_delay_us(10000); //1ms
        gpio_init();
        os_delay_us(1000); //1ms
        i2c_master_gpio_init();
        os_delay_us(1000); //1ms
        i2c_master_init();
        os_delay_us(1000); //1ms
        initAccelorometer();
        os_delay_us(1000); //1ms
        initPolutionSensor();
        os_delay_us(1000); //1ms
        initInterrupt();
        os_delay_us(1000); //1ms

    //-----------Select Pin Functions--------------
        //Output GPIOs
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);

        //Input GPIOs
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(13));


    //-----------Wifi Mode-------------
        wifi_set_opmode(STATION_MODE);         // Setting WiFi to Station Mode                 
        user_set_station_config();          // Calling Wifi Config
        os_delay_us(15000); //15ms

        wifi_station_disconnect();
        wifi_set_opmode(NULL_MODE);						 		//Set WIFI to Null Mode (Disabled)
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);						//Set sleep mode to Modem Sleep
        wifi_fpm_open();											//Enable force sleep
        os_delay_us(15000); //15ms*/
        wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);	

        os_printf("WIFI Opmode:%X\n Wifi Sleep Mode %X\n", wifi_get_opmode(), wifi_fpm_get_sleep_type());

        power_on_sound(); 

    //-----------Loop-------------
    system_os_task(loop, userProcTaskPrio, procTaskQueue, procTaskQueueLen);
    system_os_post(userProcTaskPrio, 0, 0); // */
}

//--------------------------------------------------------------
// Loop
//--------------------------------------------------------------
void loop(os_event_t* events) {
//Processes which run in both active and modem mode
    batteryADC();
    os_delay_us(100); //0.1ms
    power_mode();
    os_delay_us(100); //0.1s
    //os_printf("WIFI Opmode:%X\t Wifi Sleep Mode %X\n", wifi_get_opmode(), wifi_fpm_get_sleep_type());
    system_os_post(userProcTaskPrio, 0, 0);
}

//--------------------------------------------------------------
// Functions
//--------------------------------------------------------------
void batteryADC(void){	
    adc_val = system_adc_read();
    adc_calib_val = (float)adc_val/281;
    //os_printf("ADC Value = "); print_float(adc_calib_val); os_printf("\n");

    if(adc_calib_val < 3.0){ //If Battery voltage too low
        GPIO_OUTPUT_SET(5, 1); //turn on Red indicator 
    }
    else{
        GPIO_OUTPUT_SET(5, 0); //turn off Red indicator 
    }
}



void delay_ms(int ms){
    for(int i=0; i < ms; i++){
        os_delay_us(1000); //1 ms
    }  
}
void delay_s (int s){
    for(int i=0; i < 20*s; i++){
        os_delay_us(50000); //50 ms
    }
}


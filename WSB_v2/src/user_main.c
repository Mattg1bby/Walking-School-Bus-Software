/* Matthew Fitzgibbons 10/02/2024
 *
 * WSB v2, version where the structure of the WSB code on the ESPWROOM02 Dev Board is built, Also drivers are implement in this version
 *  for different Functions such as I2C, Accelerometer, StepCOunter and Intterupt
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

#define	FPM_SLEEP_MAX_TIME			 0xFFFFFFF

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
bool toggle = 0;
int j=0;
int play=0;
int inter_count = 0;
int step_count = 0;
uint16_t adc_val = 0;
float adc_calib_val = 0;
AccelerometerData accelData;

//--------------------------------------------------------------
// Declarations:
//--------------------------------------------------------------
void check_RSSI(void);
void user_set_station_config(void);
void handleInterrupt(void *arg);
void input_gpio(void);
void batteryADC(void);
void buzzernote(int freq);
void power_mode(void);


//void loop(os_event_t* events);
//void loop(void *arg);

//--------------------------------------------------------------
// Main:
//--------------------------------------------------------------
void ICACHE_FLASH_ATTR user_init(void)
{
        uart_init(115200, 115200);       // Setting Uart Speed
        os_printf("\n");
        os_delay_us(1000); //1ms
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
        wifi_set_opmode(NULL_MODE);							 		//Set WIFI to Null Mode (Disabled)
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);						//Set sleep mode to Modem Sleep
        wifi_fpm_open();											//Enable force sleep
        os_delay_us(15000); //15ms*/

        os_printf("WIFI Opmode:%X\n Wifi Sleep Mode %X\n", wifi_get_opmode(), wifi_get_sleep_type());


    //-----------Loop-------------
    system_os_task(loop, userProcTaskPrio, procTaskQueue, procTaskQueueLen);
    system_os_post(userProcTaskPrio, 0, 0); // */

    /*os_timer_disarm(&ptimer);
    os_timer_setfn(&ptimer, (os_timer_func_t *)loop, NULL);
    os_timer_arm(&ptimer, 200, 1);//*/
}

//--------------------------------------------------------------
// Functions Definitions
//--------------------------------------------------------------
void loop(os_event_t* events) {
//Processes which run in both active and modem mode
    batteryADC();
    os_delay_us(100); //0.1ms
    input_gpio();
    os_delay_us(100); //0.1ms

        getAccelorometerData(&accelData);
        os_delay_us(100);
        stepCounter(accelData.x, accelData.y, accelData.z);
        os_delay_us(100); //0.1ms
    //os_printf("WIFI Opmode:%X\t Wifi FPM Sleep Mode %X\n", wifi_get_opmode(), wifi_fpm_get_sleep_type());

    power_mode();
    os_delay_us(100); //0.1s

    system_os_post(userProcTaskPrio, 0, 0);
}

void input_gpio(void){	
    static uint8_t input = 0;

    input = GPIO_INPUT_GET(GPIO_ID_PIN(13));

    //os_printf("GPIO Input: %d\n", input);    // Test gpio5 input print

       if (input == 1){
        //buzzernote(500); //period in us
        
    }
       else{
        //GPIO_OUTPUT_SET(12, 0);
    }
    os_delay_us(1000);
}

void batteryADC(void){	
    adc_val = system_adc_read();
    adc_calib_val = (float)adc_val/281;
    //os_printf("ADC Value = "); print_float(adc_calib_val); os_printf("\n");

    if(adc_calib_val < 2.0){ //If Battery voltage too low
        GPIO_OUTPUT_SET(5, 1); //turn on Red indicator 
    }
    else{
        GPIO_OUTPUT_SET(5, 0); //turn off Red indicator 
    }
}

void buzzernote(int period){
   for(j=0; j<1; j++){   
        for(int i=0; i < 1000; i++){
        toggle = !toggle;
        //os_printf("Toggle : %d \t Int: %d\n",toggle, i);
        if (toggle == 1){
            GPIO_OUTPUT_SET(12, 1);
        }
        else{
            GPIO_OUTPUT_SET(12, 0);
        }
        os_delay_us(period/2);
        }
   }
}


void delay_s (uint8_t sec){
    for(int i=0; i < 20*sec; i++){
        os_delay_us(50000); //50 ms
    }
}


//------------ Wifi Func ------------
void power_mode(void){
     //if Wifi in Null Mode
    if(wifi_get_opmode() == 0){  
        play = 0; //Reset play note 
        }

    //Else Wifi On
    else{
        //Play sound to alert Wifi is on once
        if(play == 0){
            buzzernote(318); //period in us
            buzzernote(248); //period in us
            play = 1;
        }

        //Check Wifi stregth
        check_RSSI();

        //Count Steps
        getAccelorometerData(&accelData);
        os_delay_us(100);
        stepCounter(accelData.x, accelData.y, accelData.z);
        os_delay_us(100); //0.1ms
    }
 
}

void user_set_station_config(void){
    char ssid[32] = "HONOR 20e";  
    char password[64] = "BigTim!24";
 
    struct station_config   stationConf;    
    stationConf.bssid_set=  0;
 
    os_memcpy(&stationConf.ssid, ssid, 32);
    os_memcpy(&stationConf.password, password, 64);
 
    wifi_station_set_config(&stationConf);  
}
 
// Function to print current RSSI value to the terminal
void check_RSSI(void){
    sint8_t RSSI;
    RSSI = wifi_station_get_rssi();
    //os_printf("RSSI:%d\n", RSSI);

    if(RSSI < -65){
        buzzernote(500);
    }
}

/*void fpm_wakup_cb_func1(void)
{
        wifi_fpm_close();				 	 	 					//	disable	force	sleep	function
        wifi_set_opmode(STATION_MODE);							 	//	set	station	mode
        wifi_station_connect();										//	connect	to	AP
}

/*void	user_func()
{		
        wifi_station_disconnect();
        wifi_set_opmode(NULL_MODE);							 		//	set	WiFi mode null mode.
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);						//	modem sleep
        wifi_fpm_open();											//	enable force sleep

        #ifdef	SLEEP_MAX
//	For	modem	sleep,	FPM_SLEEP_MAX_TIME	can	only	be	wakened	by	calling		
//wifi_fpm_do_wakeup.	
        wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);	
#else
        //	wakeup	automatically	when	timeout.
        wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1);		//	Set	wakeup	callback	
        wifi_fpm_do_sleep(50*1000);					
#endif	
}

#ifdef	SLEEP_MAX
    void func1(void)
    {
        wifi_fpm_do_wakeup();
        wifi_fpm_close();					 	 					//	disable	force	sleep	function
        wifi_set_opmode(STATION_MODE);								//	set	station	mode
        wifi_station_connect();										//	connect	to	AP
    }
#endif

void UdpRecvCb(void *arg, char *pdata, unsigned short len)
{
	os_printf("udp message received:\n");
	os_printf("%s\r\n", pdata);
}*/

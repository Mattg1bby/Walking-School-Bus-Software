/* Matthew Fitzgibbons 10/02/2024
 *
 * WSB v1, testing interrupt outputs and inputs from the Pins on the ESPWROOM02 Dev Board
 *
 * ------------------------NOTES--------------------------------
 *
 */

#include "osapi.h"
#include "user_interface.h"
static os_timer_t ptimer;

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
volatile int toggle = 0;

//--------------------------------------------------------------
// Declarations:
//--------------------------------------------------------------
void blinky(void *arg);
void input_test();
void handleInterrupt(void *arg);
void loop(void *arg);

//--------------------------------------------------------------
// Main:
//--------------------------------------------------------------
void ICACHE_FLASH_ATTR user_init(void)
{
        gpio_init();
        i2c_master_gpio_init();

        uart_init(115200, 115200);       // Setting Uart Speed (if its nonsesne in terminal change .ini monitor speed to 115200)
        os_delay_us(15000); //15ms
        os_printf("SDK version:%s\n", system_get_sdk_version());


        // Disable WiFi
        wifi_set_opmode(NULL_MODE);
        os_delay_us(15000); //15ms
    //-----------Select Pin Functions--------------
        //Output to Green of RGB GPIO12, GPIO15
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U , FUNC_GPIO12);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U , FUNC_GPIO15);

        //Input GPIO5
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(5));
        //PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);

        //Interrupt GPIO4, GPIO13
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
        PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));

        /*PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
        PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(13));*/

    //-------------Interupt Trigger Routine-------------
        //ETS_GPIO_INTR_ATTACH(&handleInterrupt, NULL);
        
        ETS_GPIO_INTR_DISABLE();
        ETS_GPIO_INTR_ATTACH(&handleInterrupt, NULL);
        gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
        //gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_POSEDGE);
        ETS_GPIO_INTR_ENABLE();

    //------------- Input Test -------------
        os_timer_disarm(&ptimer);
        os_timer_setfn(&ptimer, (os_timer_func_t *)loop, NULL);
        os_timer_arm(&ptimer, 200, 1);
}

//--------------------------------------------------------------
// Functions Definitions
//--------------------------------------------------------------
void blinky(void *arg)
{	
    static uint8_t state = 0;

	if (state) {
		GPIO_OUTPUT_SET(2, 1);
	} else {
		GPIO_OUTPUT_SET(2, 0);
	}
	state ^= 1;
}


void input_test()
{	static uint8_t input = 0;

    input = GPIO_INPUT_GET(GPIO_ID_PIN(5));

    os_printf("GPIO Input: %d\n", input);    // Test gpio5 input print

       if (input == 1){
        //Set GPIO12 to High
        GPIO_OUTPUT_SET(12, 1);
    }
       else{
        //Set GPIO12 to High
        GPIO_OUTPUT_SET(12, 0);
    }
}

void handleInterrupt(void *arg) {
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    os_printf("Interrupt Triggered Called\n");    // Test If Trigger is called
    os_delay_us(15000); //15ms

    if (gpio_status & BIT(4)) {
        toggle = !toggle;
        os_printf("GPIO4 Interupt\n");
        GPIO_OUTPUT_SET(15, toggle);
        //disable interrupt
        gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_DISABLE);
        //ETS_GPIO_INTR_DISABLE();
        //clear interrupt status
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(4));
        gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
        //ETS_GPIO_INTR_ENABLE();
    }

    if (gpio_status & BIT(13)) {
        toggle = !toggle;
        os_printf("GPIO13 Interupt\n");
        GPIO_OUTPUT_SET(12, toggle);
        //disable interrupt
        gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_DISABLE);
        //ETS_GPIO_INTR_DISABLE();
        //clear interrupt status
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(13));
        gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_NEGEDGE);
       // ETS_GPIO_INTR_ENABLE();
    }


}

void loop(void *arg) {
 input_test();
}

void delay_s (uint8_t sec){

    for(int i=0; i<sec; i++){
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms

        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
        os_delay_us(50000); //50 ms
    }
}
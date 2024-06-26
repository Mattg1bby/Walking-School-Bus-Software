/* Matthew Fitzgibbons 01/03/2024
 *
 * This code deploys a blinky function to the ESP_WROOM_02. It was the first code
 * developed in this project.
 *
 */
#include "osapi.h"
#include "user_interface.h"

static os_timer_t ptimer;

void blinky(void *arg);
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
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

//----------Main--------------
void ICACHE_FLASH_ATTR user_init(void)
{
    //Initilisation
    gpio_init();
    uart_init(115200, 115200);
    os_printf("SDK version:%s\n", system_get_sdk_version());
 
    wifi_set_opmode(NULL_MODE);     // Disable WiFi

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);    //Confugure Pin

    //Loop to Run Blinky
    os_timer_disarm(&ptimer);
    os_timer_setfn(&ptimer, (os_timer_func_t *)blinky, NULL);
    os_timer_arm(&ptimer, 2000, 1);
}

//--------Function Definitions---------
void blinky(void *arg)
{
	static uint8_t state = 0;

	if (state) {
		GPIO_OUTPUT_SET(4, 1); //Set GPIO 4 High
	} else {
		GPIO_OUTPUT_SET(4, 0); //Set GPIO 4 Low
	}
   
	state ^= 1; //Bitwise XOR to Toggle State
}

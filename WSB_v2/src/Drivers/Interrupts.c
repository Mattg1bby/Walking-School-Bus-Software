#include "Interrupts.h"


void initInterrupt(){
        //Interrupt GPIO4, GPIO13
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
        PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));

        /*PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
        PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(13));//*/

    //-------------Interrupt Trigger Routine-------------
        //ETS_GPIO_INTR_ATTACH(&handleInterrupt, NULL);

        ETS_GPIO_INTR_DISABLE();
        ETS_GPIO_INTR_ATTACH(&handleInterrupt, NULL);
        gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
        ETS_GPIO_INTR_ENABLE();

        /*ETS_GPIO_INTR_DISABLE();
        ETS_GPIO_INTR_ATTACH(&handleInterrupt, NULL);
        gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_NEGEDGE);
        ETS_GPIO_INTR_ENABLE();//*/
}

void handleInterrupt(void *arg) {
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);;
    os_printf("Interrupt Triggered Called\n");    // Test If Trigger is called
    os_delay_us(15000); //15ms
  

    if (gpio_status & BIT(4)) {
        os_printf("GPIO4 Interupt\n");
        /*wifi_station_disconnect();
        wifi_set_opmode(NULL_MODE);							 		//Set WIFI to Null Mode (Disabled)
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);						//Set sleep mode to Modem Sleep
        wifi_fpm_open();//*/

        if(inter_count == 0){
            os_printf("Wake Up. Inter Count = %X\n", inter_count);
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_DISABLE); //disable interrupt
            //ETS_GPIO_INTR_DISABLE();

            //Wake Device Up
            wifi_fpm_do_wakeup();
            wifi_fpm_close();					 	 					// Disable Force Sleep
            wifi_set_opmode(STATION_MODE);								// Set Station Mode
            wifi_fpm_set_sleep_type(NONE_SLEEP_T);                      // Set no sleep mode
            wifi_station_connect();	
            //Debounce Time
            os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);
            //Clear interrupt status
            GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(4));
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
            //ETS_GPIO_INTR_ENABLE();

            inter_count=1;          //Increment the counter
        }
        else //if (inter_count == 1)
        {
            os_printf("Sleep. Inter Count = %X\n", inter_count);
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_DISABLE); //disable interrupt
            //ETS_GPIO_INTR_DISABLE();

            //Modem Sleep Mode Set
            wifi_station_disconnect();
            wifi_set_opmode(NULL_MODE);							 		//Set WIFI to Null Mode (Disabled)
            wifi_fpm_set_sleep_type(MODEM_SLEEP_T);						//Set sleep mode to Modem Sleep
            wifi_fpm_open();								        	// CETS_GPIO_INTR_ENABLE();onnect to AP

            //Debounce Time
            os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);os_delay_us(50000);
            //Clear interrupt status and enable interrupt
            GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(4));
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
            //ETS_GPIO_INTR_ENABLE();

            inter_count=0;          //Set couter to 0
        }
    }
}


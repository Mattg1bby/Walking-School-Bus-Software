#include "Interrupts.h"


void initInterrupt(){

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));
}

void handleInterrupt(void *arg) {
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    inter_count++;
    os_printf("Interrupt Triggered Called\n");    // Test If Trigger is called
    os_delay_us(15000); //15ms
  

    if (gpio_status & BIT(4)) {
        os_printf("GPIO4 Interupt\n");
        if(inter_count == 1){
            wifi_set_opmode(STATION_MODE);
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_DISABLE); //disable interrupt
            //clear interrupt status
            GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(4));
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
        }
        else if (inter_count == 2)
        {
            
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_DISABLE); //disable interrupt
            //clear interrupt status
            GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(4));
            gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);

            inter_count=0;
        }
        
    }

}


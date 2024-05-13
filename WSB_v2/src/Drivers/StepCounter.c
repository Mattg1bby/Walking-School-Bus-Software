#include "I2C_Func.h"
#include "Accelerometer.h"
#include "StepCounter.h"


void stepCounter(sint16_t X, sint16_t Y, sint16_t Z){
//os_printf("%d,%d,%d,",X,Y,Z);

//Protection limiters to stop a step being counted when its just an orientation change
 if((-2000 < Y && Y < 2000) && (-2000 < Z && Z < 2000) && ((3000 < X && X < 5000)||(-5000 < X && X < -3000))){
    //then If threshold exceeded then count a step
    if ((X+4000) > 1500 ){
    step_count++;
    //debounce timer 250ms
    os_delay_us(50000); os_delay_us(50000); os_delay_us(50000); 
    os_delay_us(50000); os_delay_us(50000); 
    os_printf("Steps : %d \n", step_count);
    }

 }
}
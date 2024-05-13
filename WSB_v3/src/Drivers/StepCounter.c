#include "I2C_Func.h"
#include "Accelerometer.h"
#include "StepCounter.h"



void stepCounter(sint16_t X, sint16_t Y, sint16_t Z){
    //os_printf("%d,%d,%d,",  X,Y,Z);
    //os_printf("%d\t%d\t%d\n",  X,Y,Z);
//Protection limiters to stop a step being counted when its just an orientation change
 if((-1500 < Y && Y < 1500) && (-1500 < Z && Z < 1500)){
    
    //If X facing Up
    if((2000 < X && X < 6000)){
        if (X > 5000 ){
            step_count++;
            //debounce timer 200ms
            os_delay_us(50000); os_delay_us(50000); os_delay_us(50000); 
            os_delay_us(50000); os_delay_us(50000); 
            os_printf("Steps : %d \n", step_count);

                  if(step_count%10 == 0){
                   ten_step_alert();
                  }
        }
    }
    //If X Facing Down
    else if (-6000 < X && X < -2000){
        if (X < -4800 ){
            step_count++;
            //debounce timer 300ms (Longer than in StepCounter due to faster Loop)
            os_delay_us(50000); os_delay_us(50000); os_delay_us(50000); 
            os_delay_us(50000); os_delay_us(50000); os_delay_us(50000); 
            os_printf("Steps : %d \n", step_count);

                  if(step_count%10 == 0){
                   ten_step_alert();
                  }
        }
   }//*/
      
   }
}
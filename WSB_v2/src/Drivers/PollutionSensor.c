#include "I2C_Func.h"
#include "PollutionSensor.h"


void initPolutionSensor(){  
    
    int8_t test_byte = 0;

    os_printf("---- Init Pollution Sensor Start ----\r\n");

    os_printf("---- Init Pollution Sensor End ----\r\n");
    /*int8_t test_byte = 0;
    //Write to a writable address
        beginTransmission(ZMOD4510Address);
        if(checkAck(ack) == TRUE){
        write(0x88); //Register Address   
        }
        if(checkAck(ack) == TRUE){
        write(0x53); //Writeable Addresses 0x88 to 0x8B to test I2C Configured   
        }
        if(checkAck(ack) == TRUE){
        endTransmission();  
        }
        os_delay_us(100);*/

    //Read address just written to
        //os_printf("---- Read data Start ----\r\n");
        beginTransmission(ZMOD4510Address);
        requestFrom(0x88,ZMOD4510Address);//Request to Read
        singleByteReadEnd(&test_byte); //Read Address
        endTransmission(); //End transmission
        os_printf("Read Register Polution Sensor 0x88: 0x%X\r\n", test_byte);
        //os_printf("---- Read data End----\r\n");
        os_delay_us(100); //0.1ms*/
}

#include "I2C_Func.h"
#include "Accelerometer.h"

void initAccelorometer(){ 
    uint8 byte = 0;
    //Reset MMA8451QAddress
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == TRUE){
        write(0x2B); //Register Address   
        }
        if(checkAck(ack) == TRUE){
        write(0x40); //Data - Reset Device - 0100 0000   
        }
        if(checkAck(ack) == TRUE){
        endTransmission();  
        }
        os_delay_us(1000);

    //Set to Standby Mode
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == TRUE){
        write(0x2A); //Register Address   
        }
        if(checkAck(ack) == TRUE){
        write(0x00); //Data - Set to Standby mode - 00000000   
        }
        if(checkAck(ack) == TRUE){
        endTransmission();  
        }
        os_delay_us(100);
    
    //Sets the F setup register 
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == TRUE){
        write(0x09); //Register Address
        }
        if(checkAck(ack) == TRUE){
        write(0x00); //Data - FIFO Disabled - 00000000   
        }
        if(checkAck(ack) == TRUE){
        endTransmission();  
        }
        os_delay_us(100);

    //Sets the dynamic range 
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == TRUE){
        write(0x0E); //Register Address
        }
        if(checkAck(ack) == TRUE){
        write(0x00); //Data - Set dynamic range 2g - 00000000   
        }
        if(checkAck(ack) == TRUE){
        endTransmission();  
        }
        os_delay_us(100);

    //Set to Active Mode
        beginTransmission(MMA8451QAddress);
        if(checkAck(ack) == TRUE){
        write(0x2A); //Register Address   
        }
        if(checkAck(ack) == TRUE){
        write(0x01); //Data - Set to Active mode and Fast Read mode (1 Byte) - 00000001   
        }
        if(checkAck(ack) == TRUE){
        endTransmission();  
        }
        os_delay_us(100);

        //Test Reading WHO_AM_I register
        //os_printf("---- Read data Start ----\r\n");
        beginTransmission(MMA8451QAddress);
        requestFrom(0x0D,MMA8451QAddress);//Request to Read - Bug in this functions
        singleByteReadEnd(&byte); //Read Address
        endTransmission(); //End transmission
        os_printf("Read data WHO_AM_I 0x0D: 0x%X\r\n", byte);
        //os_printf("---- Read data End----\r\n");
        os_delay_us(100); //0.1ms*/
}

void getAccelorometerData(AccelerometerData *data)
{
    bool XYZDR_Flag = 0;
    int8_t F_Status = 0;

    int8_t X_out_MSB = 0;
    int8_t X_out_LSB = 0;
    sint16_t X_out = 0;

    int8_t Y_out_MSB = 0;
    int8_t Y_out_LSB = 0;
    sint16_t Y_out = 0;

    int8_t Z_out_MSB = 0;
    int8_t Z_out_LSB = 0;
    sint16_t Z_out = 0;

    //os_printf("---- Get Accelorometer Start ----\r\n");

    //Read F
    beginTransmission(MMA8451QAddress);
    requestFrom(0x00,MMA8451QAddress);//Request to Read F Status
    singleByteReadEnd(&F_Status); //Read Address
    endTransmission(); //End transmission
    //os_printf("F status: 0x%X\r\n", F_Status);
    os_delay_us(100); //0.1ms

    XYZDR_Flag = (F_Status & 0x08)>>3;
    //os_printf("XYZDR_Flag: %d\r\n", XYZDR_Flag);

    if(XYZDR_Flag == 1){
       
        beginTransmission(MMA8451QAddress);
        requestFrom(0x01,MMA8451QAddress);//Request to OUT_X_MSB
        singleByteRead(&X_out_MSB); //Auto Increments through the next registers
        singleByteRead(&X_out_LSB);
        singleByteRead(&Y_out_MSB);
        singleByteRead(&Y_out_LSB);
        singleByteRead(&Z_out_MSB);
        singleByteReadEnd(&Z_out_LSB);
        endTransmission(); //End transmission
        os_delay_us(100); //0.1ms
       
        X_out = convert(X_out_MSB, X_out_LSB);
        Y_out = convert(Y_out_MSB, Y_out_LSB);
        Z_out = convert(Z_out_MSB, Z_out_LSB);

        /*os_printf("OUT_X_MSB: 0x%X\r\n", X_out_MSB);
        os_printf("OUT_X_LSB: 0x%X\r\n", X_out_LSB);

        os_printf("OUT_Y_MSB: 0x%X\r\n", Y_out_MSB);
        os_printf("OUT_Y_LSB: 0x%X\r\n", Y_out_LSB);

        os_printf("OUT_Z_MSB: 0x%X\r\n", Z_out_MSB);
        os_printf("OUT_Z_LSB: 0x%X\r\n", Z_out_LSB);//*/

    //Print Values between -100 and 100 to get more readable values
       //print_float(X_out/82.0); os_printf(", "); print_float(Y_out/82.0); os_printf(", "); print_float(Z_out/82.0); os_printf(", ");
        
        /*os_printf("OUT_X: "); print_float(X_out/82.0); os_printf("\t");
        os_printf("OUT_Y: "); print_float(Y_out/82.0); os_printf("\t");
        os_printf("OUT_Z: "); print_float(Z_out/82.0); os_printf("\n");//*/

    //Print out raw 14 bit values
        //os_printf("%d, ",  X_out); os_printf("%d, ",  Y_out);  os_printf("%d, ",  Z_out);
        //os_printf("%d, ",  X_out_offset);

        /*os_printf("OUT_X: %d \t", X_out);
        os_printf("OUT_Y: %d \t", Y_out); 
        os_printf("OUT_Z: %d \n", Z_out);//*/ 

        data->x = X_out;
        data->y = Y_out;
        data->z = Z_out;
    }
    else{
        os_printf("No new data\r\n");
    }

//os_printf("---- Get Accelorometer End ----\r\n");
}

sint16_t convert (uint8_t MSB, uint8_t LSB){
    
    uint16_t newMSB = MSB;
    uint16 newLSB = LSB;
    newMSB = newMSB << 8;
    uint16_t data = (newMSB + newLSB) >> 2;

    if(MSB & 0x80){
        return data + 0xC000;
     }
    else{
        return data;
    }
   
}

void print_float (float value){
    int16_t wholePart = (int)value;
    int16_t decimalPart = (int)((value-wholePart)*10000);


    if (value < 0) {
        decimalPart = -decimalPart;
    }

    os_printf("%d.", wholePart);
    //if statements to print 0 demcimal parts
    if (decimalPart < 1000) {
        os_printf("0");
    }
    if (decimalPart < 100) {
        os_printf("0"); 
    }
    if (decimalPart < 10) {
        os_printf("0"); 
    }

    os_printf("%d", decimalPart);
}


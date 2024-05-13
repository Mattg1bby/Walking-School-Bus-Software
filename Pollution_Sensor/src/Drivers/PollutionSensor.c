#include "I2C_Func.h"
#include "PollutionSensor.h"

#include "zmod4xxx.h"

void initPolutionSensor(){  
    
    int8_t ret;
    zmod4xxx_dev_t dev;
    

    /* Sensor specific variables */
    uint8_t zmod4xxx_status;
    uint8_t track_number[ZMOD4XXX_LEN_TRACKING];
    uint8_t adc_result[ZMOD4510_ADC_DATA_LEN];
    uint8_t prod_data[ZMOD4510_PROD_DATA_LEN];
    oaq_2nd_gen_handle_t algo_handle;
    oaq_2nd_gen_results_t algo_results;
    oaq_2nd_gen_inputs_t algo_input;

    /****TARGET SPECIFIC FUNCTION ****/
    /*
	* To allow the example running on customer-specific hardware, the init_hardware
	* function must be adapted accordingly. The mandatory funtion pointers *read,
	* *write and *delay require to be passed to "dev" (reference files located
	* in "dependencies/zmod4xxx_api/HAL" directory). For more information, read
	* the Datasheet, section "I2C Interface and Data Transmission Protocol".
    */
    ret = init_hardware(&dev);
    if (ret) {
        printf("Error %d during initialize hardware, exiting program!\n", ret);
    }
    /****TARGET SPECIFIC FUNCTION ****/

    /* Sensor related data */
    dev.i2c_addr = ZMOD4510_I2C_ADDR;
    dev.pid = ZMOD4510_PID;
    dev.init_conf = &zmod_oaq2_sensor_cfg[INIT];
    dev.meas_conf = &zmod_oaq2_sensor_cfg[MEASUREMENT];
    dev.prod_data = prod_data;

    /* Read product ID and configuration parameters. */
    ret = zmod4xxx_read_sensor_info(&dev);
    if (ret) {
        printf("Error %d during reading sensor information, exiting program!\n",
               ret);
        goto exit;
    }

    /*
	* Retrieve sensors unique tracking number and individual trimming information.
	* Provide this information when requesting support from Renesas.
    */

    ret = zmod4xxx_read_tracking_number(&dev, track_number);
    if (ret) {
        printf("Error %d during reading tracking number, exiting program!\n",
               ret);
        goto exit;
    }
    printf("Sensor tracking number: x0000");
    for (int i = 0; i < sizeof(track_number); i++) {
        printf("%02X", track_number[i]);
    }
    printf("\n");
    printf("Sensor trimming data: ");
    for (int i = 0; i < sizeof(prod_data); i++) {
        printf(" %i", prod_data[i]);
    }
    printf("\n");

	/*
    * Start the cleaning procedure. Check the Programming Manual on indications
	* of usage. IMPORTANT NOTE: The cleaning procedure can be run only once
	* during the modules lifetime and takes 10 minutes (blocking).
    */

    //ret = zmod4xxx_cleaning_run(&dev);
    //if (ret) {
    //    printf("Error %d during cleaning procedure, exiting program!\n", ret);
    //    goto exit;
    //}

    /* Determine calibration parameters and configure measurement. */
    ret = zmod4xxx_prepare_sensor(&dev);
    if (ret) {
        printf("Error %d during preparation of the sensor, exiting program!\n",
               ret);
        goto exit;
    }

    /*
    * One-time initialization of the algorithm. Handle passed to calculation
    * function.
    */
    ret = init_oaq_2nd_gen(&algo_handle);
    if (ret) {
        printf("Error %d during initializing algorithm, exiting program!\n",
                ret);
        goto exit;
    }

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

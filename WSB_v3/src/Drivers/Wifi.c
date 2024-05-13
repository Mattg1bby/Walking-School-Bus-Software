#include "Wifi.h"


//------------ Wifi Func ------------
void power_mode(void){
     //if Wifi in Null Mode
    if(wifi_get_opmode() == 0){  
        arm_wifi_on_sound = 1; //Reset play note 
        arm_connect_sound = 1; //Reset play note

        if(arm_wifi_off_sound == 1){
            arm_wifi_off_sound = 0;
            wifi_off_sound();
            //os_printf("Wifi off Sound \n");
        }
    }

    //Else Wifi On
    else{
        arm_wifi_off_sound = 1; //Reset play note  

        //Play sound to alert Wifi is on once
        if(arm_wifi_on_sound == 1){
            arm_wifi_on_sound = 0;
            wifi_on_sound();
             //os_printf("Wifi on Sound \n");
        }

        if (wifi_station_get_connect_status() == STATION_GOT_IP){
            //Check Wifi stregth
            check_RSSI();
                if(arm_connect_sound == 1){
                    arm_connect_sound = 0;
                    device_paired_sound();
                    //os_printf("Device Paired Sound \n");
                }
        }

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
    uint8_t RSSI;
    //RSSI = wifi_station_get_rssi();
    RSSI = updateRSSI();
    //os_printf("RSSI:%d\n", RSSI);

    if(RSSI > 60){
        out_of_range_alarm();
        //power_on_sound(); //For some reason 
    }
}

/*

@ wifi.c
@ Provides methods and variables about a Wi-Fi station mode
  connection to an access point (AP) to configure connection 
  settings bla bla bla bla #TODO...
@ Sean Quinn Jones
@ 13/04/2024
*/

//#include "wifi.h"

// ================== // Functional Variables and Prototypes // ================ //
                                
static os_timer_t ptimer;                           // OS Timer created to allow RSSI Measurments

struct station_config stationConf;                  // The station configuration
struct espconn udpTrans;                            // The transmission structure
esp_udp espUDP;                                     // The UDP transmission configuration
struct scan_config scanConf;                        // Scan configuration structure
struct bss_info *bss_link;

void wifiHandler(System_Event_t *evt);              // Wi-Fi Handler callback
void wifiScanCallback(void *arg, STATUS status);    // The Wi-Fi scan callack
void ipToArray(uint32 ip, uint8* ipArray);          // IP conversion tool
void printIpArray(uint8* ipArray);                  // Serially prints an IP array
void configureUDP(void);                            // UDP configuration function

uint8_t updateRSSI(void);                              // The timer activated RSSI update

// Recieving callback function
void udpRecvCallback(void *arg,	
                     char *pdata, 
                     unsigned short len);

float rssiArray[AVERAGENUM] = {0};                  // A rolling average array of RSSI values
uint8 arrayPointer = 0;                             // The current array pointer
float rssiSum = 0;                                  // The sum of all RSSI points

// ============================ // Wi-Fi Methods // ============================ //

/*
    configureWifi()

    The main Wi-Fi configuration function 
    when called, it configures various basic
    Wi-Fi parameters
    Note: Does not form a connection to server
          For connection, use connectWifi
*/
bool configureWifi(){

    // Antenna power is set (unit: 0.25 dBm, range [0, 82])
    system_phy_set_max_tpw(82);       

    // Wi-Fi handler callback is set
    wifi_set_event_handler_cb(wifiHandler); 

    // Wi-Fi 4 is set
    if(!wifi_set_phy_mode(PHY_MODE_11N)){                   
        os_printf("ERROR: Physical Mode Setting\n");
        return false;
    } else {os_printf("SUCCESS: Physical Mode Setting\n");}                      

    // Operation mode is set to Station
    if(!wifi_set_opmode(STATION_MODE)){                          
        os_printf("ERROR: Operation Mode Setting");
        return false;
    } else {os_printf("SUCCESS: Operation Mode Setting\n");}

    // Auto-Connect on powerup is set to 0 to initially prevent automatic connection
    if(!wifi_station_set_auto_connect(0)){                          
        os_printf("ERROR: Auto-Connect Mode Setting\n");
        return false;
    } else {os_printf("SUCCESS: Auto-Connect Mode Setting\n");}

    // Disconnection retry policy (to try and reconnect or not after disconnection)
    if(!wifi_station_set_reconnect_policy(true)){                         
        os_printf("ERROR: Retry Mode Setting\n");
        return false;
    } else {os_printf("SUCCESS: Retry Mode Setting\n");}

    // DHCP disconnection retry number (hoq many times reconnection will be attempted)
    if(!wifi_station_dhcpc_set_maxtry(255)){                          
        os_printf("ERROR: DHCP Max-Retry Mode Setting\n");
        return false;
    } else {os_printf("SUCCESS: DHCP Max-Retry Mode Setting\n");}

    // DHCP disconnection retry number (hoq many times reconnection will be attempted)
    if(!wifi_station_set_hostname(HOSTNAME)){                          
        os_printf("ERROR: Hostname Setting\n");
        return false;
    } else {os_printf("SUCCESS: Hostname Setting\n");}
                         
    // If successful, user is alterted 
    os_printf("Wi-Fi Configuration SUCCESSFUL\n");

                // ==== // Setting Station Config // ==== //

    // In the event of no errors, returns true
    return true;    
}


/*
    connectWifi()

    This creates a connection to a server
    with given SSID and password
    Note: Must be called after configureWifi
*/
bool connectWifi(char* SSID, char* PASS){

    os_printf("Attempting Wi-Fi Connection\n");

    // ==== // Setting Station Config // ==== //

    // Set to 0 to prevent AP/server needing a particular MAC address
    stationConf.bssid_set =	0;

    // SSID and Password written to the station configuration
	os_memcpy(&stationConf.ssid, SSID, os_strlen(SSID));	
    os_memcpy(&stationConf.password, PASS, os_strlen(PASS));

    // Scan Configuration filled in once SSID and Password are known
    os_memcpy(&scanConf.ssid, SSID, os_strlen(SSID));
    scanConf.bssid = NULL;                              // Don't care about MAC addresses
    scanConf.channel = 0;                               // All channels scanned
    scanConf.show_hidden = 1;                           // Hidden networks scanned
    scanConf.scan_type = WIFI_SCAN_TYPE_ACTIVE;         // Active scan used

    // Auto Connect Wi-Fi enabled to make ESP8266 connect
    if(!wifi_station_set_auto_connect(1)){                          
        os_printf("ERROR: Auto-Connect Mode Setting\n");
        return false;
    } else {os_printf("SUCCESS: Auto-Connect Mode Setting\n");}

    // Station configuration type is written
    return wifi_station_set_config_current(&stationConf);

    /*
    Following the config, the ESP8266 will automatically attempt
    to connect to the server 
    */

}


/*
    configureUDP()

    This configures a UDP transmission to
    the server with locally stored port numbers 
    and local port number and IP addresses
*/
void configureUDP(void){

    // Setting the ESP's local port value
    espPort = ESPPORT;
    serverPort = SERVERPORT;

    espUDP.local_port = espPort;
    espUDP.remote_port = serverPort;

    os_memcpy(&espUDP.local_ip, espStationIpArray, 4);
    os_memcpy(&espUDP.remote_ip, serverIpArray, 4);

    // Setting transmission type as UDP
    udpTrans.type = ESPCONN_UDP;
    udpTrans.state = ESPCONN_NONE;
    udpTrans.proto.udp = &espUDP;

    // UDP Transmission is created
    espconn_create(&udpTrans);
}


/*
    sendUDPMessage()

    Compiles a message to send the android phone and
    sends it basup upon local IP address, data type
    and data 
*/
sint16 sendUDPMessage(enum udpMessageType messageType, sint8 data){
    uint8 message[6] = {                            // Byte:            // Data:
                        espStationIpArray[0],       //      1           first byte of IP
                        espStationIpArray[1],       //      2           second byte of IP
                        espStationIpArray[2],       //      3           third byte of IP
                        espStationIpArray[3],       //      4           fourth byte of IP
                        messageType,                //      5           data type being sent
                        data                        //      6           value of data type
                    };

    // Message is transmistted through UDP
    espconn_sendto(&udpTrans, message, sizeof(message));
}

/*
    startDistanceRanging()

    Starts the process of determining RSSI 
    and range data and sending it to the
    server 

void startDistanceRanging(){

    os_timer_disarm(&ptimer);
    os_timer_setfn(&ptimer, (os_timer_func_t *)updateRSSI, NULL);
    os_timer_arm(&ptimer, AVERAGEPERIOD, 1);
}*/


/*
    ipToArray()

    Converts the raw uint32 IP address into a
    4 element uint8 array for use in functions
*/
void ipToArray(uint32 ip, uint8* ipArray){

    // Extract the bytes for each element of the IP
    ipArray[0] = ip4_addr1(ip);
    ipArray[1] = ip4_addr2(ip);
    ipArray[2] = ip4_addr3(ip);
    ipArray[3] = ip4_addr4(ip);
}


/*
    printIpArray()

    Prints to terminal, the argument IP Array
*/
void printIpArray(uint8* ipArray){

    os_printf("Conveted IP: %d", ipArray[0]);
    os_printf(" : %d", ipArray[1]);
    os_printf(" : %d", ipArray[2]);
    os_printf(" : %d\n", ipArray[3]);
}


// ========================= // Wi-Fi Callback Methods // ========================= //

/*
    wifiHandler()

    Upon a Wi-Fi event (Connection, disconnection etc...)
    This callback will run 
*/
void wifiHandler(System_Event_t *evt){

	switch	(evt->event)	{
		case EVENT_STAMODE_CONNECTED:

            break;

        case EVENT_STAMODE_GOT_IP:
        
            /*
                When connection exchnages IP addresses, they are extracted from
                the system event argument, they are converted to an array form
                and stored in global variables. 

                Following this, the ESP MAC address is found and a UDP 
                transmission is created to send a welcome message
            */
           
           // IP addresses are extracted as raw uint32
            espStationIp = (uint32)&evt->event_info.got_ip.ip.addr;
            serverIp = (uint32)&evt->event_info.got_ip.gw.addr;

            // Raw uint32 addresses are printed
            os_printf("ESP RAW IP: %d\n", espStationIp);
            os_printf("SERVER RAW IP: %d\n", serverIp);

            // Raw uint32 addresses are converted to array addresses
            ipToArray(espStationIp, espStationIpArray);
            ipToArray(serverIp, serverIpArray);

            // Storing the local MAC address
            wifi_get_macaddr(STATION_IF, espStationMAC);

            // UDP Transmission is setup
            configureUDP();

            // Setting Recieve callback function
            espconn_regist_recvcb(&udpTrans, udpRecvCallback);

            // Initial Connection Message Sent
            sendUDPMessage(NONE, 0xAB);
    }
}


/*
    udpRecvCallback()

    This is the callback that operates in the
    event that a UDP packet is recieved, it 
    passes information about the packet
    for use
*/
void udpRecvCallback(void *arg,	char *pdata, unsigned short len){

    os_printf("Recieved Data: %s\n", pdata, len);
}



/*
    updateRSSI()

    This is the callback function for a
    software timed rolling averge calculator to
    smooth out RSSI values
    Note: To start the rolling average, call
          startDistanceRanging()
*/
uint8_t updateRSSI(void){

    // If end of array is reached, pointer is reset
    if(arrayPointer == AVERAGENUM){
        arrayPointer = 0;
    }

    rssiSum -= rssiArray[arrayPointer];                     // Old value is subtracted 
    rssiArray[arrayPointer] = -wifi_station_get_rssi();     // New RSSI value is written
    rssiSum += rssiArray[arrayPointer];                     // New value is added

    arrayPointer++;         // Array pointer is incremented

    // Average is taken
    rssiAverage = (rssiSum/(float)AVERAGENUM);// If end of array is reached, pointer is reset

    sendUDPMessage(RSSI, rssiAverage);

    return rssiAverage;
}

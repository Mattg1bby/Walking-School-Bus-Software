/*

@ wifi.h
@ Provides methods and variables about a Wi-Fi station mode
  connection to an access point (AP) to configure connection 
  settings bla bla bla bla #TODO...
@ Sean Quinn Jones
@ 13/04/2024
*/

#ifndef WIFI_H
#define WIFI_H

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "Accelerometer.h"

void power_mode(void);

void user_set_station_config(void);

void check_RSSI(void);
// ================================ // User Parameters // =============================== //

#define HOSTNAME "WSB_Device"         // The device hostname
#define ESPPORT 50003                 // The port used by the ESP
#define SERVERPORT 60000              // The port used by the Server
#define AVERAGENUM 1                 // The number of points to use in rolling rssi average
#define AVERAGEPERIOD 200             // The milliseconds between rolling average updates


// ========================== // Wi-Fi Connection Variables // ========================== //

static os_timer_t ptimer;           // OS Timer created to allow RSSI Measurments

uint32 espStationIp;                // The local IP address of the ESP8266 (uint32)
uint8 espStationIpArray[4];         // The local IP address of the ESP8266 as an array
uint8 espStationMAC[6];             // The local ESP8266 MAC address

uint32 serverIp;                    // The server IP address (uint32)
uint8 serverIpArray[4];             // The server IP address as an array

uint32 espPort;                     // The UDP port of the esp
uint32 serverPort;                  // The UDP port of the server

uint8 rssiAverage;         // The current RSSI average

bool arm_wifi_on_sound;
bool arm_wifi_off_sound;
bool arm_connect_sound;
AccelerometerData accelData;
// =========================== // ENUM types for data sending //==========================//


/*
    ENUM: udpMessageType

    This is the main agreed upon list of data
    types to be send from either device to 
    phone or phone to device
*/
enum udpMessageType{

    NONE,               // Effectivly a null message, data byte contains no information
    RSSI,               // RSSI of ESP 
    DISTANCE,           // ESP calculated distance
    ALARM_STATE,        // Current ESP Alarm State
    ALARM_TRIGGER,      // ESP alarm trigger
};

// ================================= // Wi-Fi Methods // ================================ //

/*
    configureWifi()

    The main Wi-Fi configuration function 
    when called, it configures various basic
    Wi-Fi parameters
    Note: Does not form a connection to server
          For connection, use connectWifi
*/
bool configureWifi();


/*
    connectWifi()

    This creates a connection to a server
    with given SSID and password
    Note: Must be called after configureWifi
*/
bool connectWifi(char* SSID, char* PASS);


/*
    sendUDPMessage()

    Compiles a message to send the android phone and
    sends it basup upon local IP address, data type
    and data 
*/
sint16 sendUDPMessage(enum udpMessageType messageType, sint8 data);


/*
    startDistanceRanging()

    Starts the process of determining RSSI 
    and range data and sending it to the
    server 
*/
void startDistanceRanging();

uint8_t updateRSSI();

void power_mode(void);

void user_set_station_config(void);

void check_RSSI(void);

#endif
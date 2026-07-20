#pragma once
#include <Arduino.h>
#include <Wire.h>


#define SDA_PIN 6
#define SCL_PIN 7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIRE &Wire
#define SCREEN_REFRESH 500 //milliseconds

//#define PIR_MOTION 1

#define SONAR_TRIG 3
#define SONAR_ECHO 4
#define SONAR_DISTANCE 200

#define BUZZER 5
#define BUTTON 1

#define IR_SEND 13
#define IR_RECIEVE 14


const char *SSID = "SSDID HERE";
const char *PASSWD = "PASSWORD HERE";

// Weather API stuff
const String APIKEY = "API THINGY";
const String LAT = "40.7179";
const String LONG = "-74.0138";
const String UNITS = "imperial";
String weatherAPI = "http://api.openweathermap.org/data/2.5/weather?lat="+LAT+"&lon="+LONG+"&units="+UNITS+"&appid="+APIKEY;

const String discordWebhook = "WEBHOOK URL HERE";

struct Status {
    bool humanPresent = false;
    bool isRaining = false;
    bool umbrellaPresent = false;
    bool walletPresent = false;

    bool missingStuff = false;
    
    String mainWeather = "";
    String description = "";
}
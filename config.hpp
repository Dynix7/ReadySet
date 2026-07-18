#include <Arduino.h>
#include <Wire.h>
#pragma once

#define SDA_PIN 6
#define SCL_PIN 7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIRE &Wire


#define PIR_MOTION 1
#define FORCE_SENSOR 2
#define SONAR_TRIG 3
#define SONAR_ECHO 4
#define SONAR_DISTANCE 200

#define BUZZER 5

// if using spi for RFID
#define SS_PIN 8
#define SCK_PIN 9
#define MOSI_PIN 10
#define MISO_PIN 11
#define RST_PIN 12

const char *SSID = "SSDID HERE";
const char *PASSWD = "PASSWORD HERE";

// Weather API stuff
const String APIKEY = "API THINGY";
const String LAT = "40.7179";
const String LONG = "-74.0138";
const String UNITS = "imperial";
String weatherAPI = "http://api.openweathermap.org/data/2.5/weather?lat="+LAT+"&lon="+LONG+"&units="+UNITS+"&appid="+APIKEY;


struct Status {
    bool isRaining = false;
    bool keysPresent = false;
    bool umbrellaPresent = false;
    bool walletPresent = false;

    String mainWeather = "";
    String description = "";
}
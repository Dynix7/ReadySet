#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <NewPing.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>

#include "config.hpp"


Adafruit_SSD1306 SCREEN(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIRE, SCREEN_RESET);
NewPing SONAR(SONAR_TRIG, SONAR_ECHO, SONAR_DISTANCE);
struct Status currentStatus;


unsigned long weatherUpdateInterval = 600000;
unsigned long prevUpdate = 0;


void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

    connectWifi();
    updateWeather();

}


void loop() {


}


void connectWifi() {
    WiFi.begin(SSID, PASSWD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println(" Connected");
}

void updateWeather() {
    if ((millis() - prevUpdate) > weatherUpdateInterval) {
        prevUpdate = millis();

        if (WiFi.status == WL_CONNECTED) {
            HTTPClient http;
            http.begin(weatherAPI);
            int httpCode = http.GET();

            if (httpCode == 200) {
                String response = http.getString();

                JsonDocument doc;
                deserializeJson(doc, response);

                String mainWeather = String(doc["weather"][0]["main"]);
                String description = String(doc["weather"][0]["description"]);

                currentStatus.mainWeather = mainWeather;
                currentStatus.description = description;

                mainWeather.toLowerCase();
                description.toLowerCase();

                if (mainWeather == "rain" || mainWeather == "drizzle") {
                    currentStatus.isRaining = true;
                } else {
                    currentStatus.isRaining = false;
                }
            }
            http.end();    
        }
    }
}

int pingSonar(NewPing *pSonar) {
    int distance = pSonar->ping_cm();
    return distance;
}

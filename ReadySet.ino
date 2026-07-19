#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <NewPing.h>
#include <Adafruit_SSD1306.h>


#include "config.hpp"
#include "Screen.hpp"

struct Status currentStatus;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIRE, SCREEN_RESET);
NewPing SONAR(SONAR_TRIG, SONAR_ECHO, SONAR_DISTANCE);

ScreenHelper SCREEN(&oled, &currentStatus);


void setup() {
    Serial.begin(115200);
    pinSetup();
    Wire.begin(SDA_PIN, SCL_PIN);
    //SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

    SCREEN.initScreen();

    connectWifi();
    updateWeather();

}


void loop() {
    updateWeather();
    Screen.updateScreen();
    

}

void pinSetup() {
    pinMode(SONAR_ECHO, INPUT);
    pinMode(SONAR_TRIG, OUTPUT);
    
    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON, INPUT);

    pinMode(IR_SEND, OUTPUT);
    pinMode(IR_RECIEVE, INPUT);
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
    const unsigned long weatherUpdateInterval = 600000;
    static unsigned long prevWeatherUpdate = 0;

    if ((millis() - prevUpdate) >= weatherUpdateInterval) {
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

bool pingIR() {
    const unsigned long IRUpdateInterval = 50;
    static unsigned long prevIRUpdate = 0;

    if ((millis() - prevIRUpdate) >= IRUpdateInterval) {
        prevIRUpdate = millis();

        tone(IR_SEND, 38000;)
        delay(2);

        int recieveReading = digitalRead(IR_RECIEVE);
        noTone(IR_SEND);

        if (recieveReading == HIGH) {
            return true; //Beam Break
        }
    }
    return false; // No Beam Break
}


void Buzz(bool startBuzz) {
    unsigned long buzzDuration = 500;
    static bool isBuzzing = false;
    static unsigned long prevBuzz = 0;

    if (startBuzz && !isBuzzing) {
        digitalWrite(BUZZER, HIGH);
        isBuzzing = true;
        prevBuzz = millis();
    }

    if (isBuzzing) {
        if ((millis() - prevBuzz) > buzzDuration) {
            digitalWrite(BUZZER, LOW);
            isBuzzing = false;
        }
    }   
}

bool checkButton(int pin) {
    const int debounceDelay = 50;
    static int prevState = HIGH;
    static unsigned long lastDebounce = 0;
    bool pressed = false;

    int reading = digitalRead(BUTTON);

    if (reading != prevState) {
        lastDebounce = millis();
    }

    if ((millis() - lastDebounce) > debounceDelay) {
        pressed = reading;
        prevState = reading;
    }
    return pressed;
}


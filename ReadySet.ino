#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <NewPing.h>
#include <Adafruit_SSD1306.h>
#include <arduino-timer.h>

#include "config.hpp"
#include "Screen.hpp"

struct Status currentStatus;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIRE, SCREEN_RESET);
NewPing SONAR(SONAR_TRIG, SONAR_ECHO, SONAR_DISTANCE);

ScreenHelper SCREEN(&oled, &currentStatus);

auto timer = timer_create_default();

void setup() {
    Serial.begin(115200);
    pinSetup();
    Wire.begin(SDA_PIN, SCL_PIN);
    //SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

    SCREEN.initScreen();

    connectWifi();
    updateWeather();

}


//Things to add
//Weekday only stuff
// Temperature Reminders
void loop() {
    static bool reminderSet = false;

    if (WiFi.status() != WL_CONNECTED) 
        connectWifi();
    
        
    timer.tick();
    SCREEN.updateScreen();
    updateWeather();
    int sonarDistance = pingSonar();
    bool beamBreak = pingIR();
    bool pressurePlate = checkButton(BUTTON);

    currentStatus.humanPresent = (sonarDistance > 0 && sonarDistance < 100);
    currentStatus.walletPresent = pressurePlate;
    currentStatus.umbrellaPresent = beamBreak;

    currentStatus.missingStuff = (!currentStatus.walletPresent || (!currentStatus.umbrellaPresent && currentStatus.isRaining));
    
    Buzz(currentStatus.humanPresent && currentStatus.missingStuff);
    if (currentStatus.humanPresent && currentStatus.missingStuff) {
        

        if (!reminderSet) {
            timer.in(180000, sendReminder);
            reminderSet = true;
        }
    } else {
        reminderSet = false;
    }

}

void pinSetup() {
    pinMode(SONAR_ECHO, INPUT);
    pinMode(SONAR_TRIG, OUTPUT);
    
    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    pinMode(IR_SEND, OUTPUT);
    pinMode(IR_RECEIVE, INPUT);
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
    static bool firstRun = true;
    if ((millis() - prevWeatherUpdate) >= weatherUpdateInterval || firstRun) {
        firstRun = false;
        prevWeatherUpdate = millis();

    
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(weatherAPI);
            int httpCode = http.GET();

            if (httpCode == 200) {
                String response = http.getString();

                JsonDocument doc;
                deserializeJson(doc, response);

                String mainWeather = (doc["weather"][0]["main"]).as<String>();
                String description = (doc["weather"][0]["description"]).as<String>();

                currentStatus.mainWeather = mainWeather;
                currentStatus.description = description;

                String mainLower = mainWeather;
                mainLower.toLowerCase();

                if (mainLower == "rain" || mainLower == "drizzle" || mainLower == "thunderstorm") {
                    currentStatus.isRaining = true;
                } else {
                    currentStatus.isRaining = false;
                }
            }
            http.end();    
        }
    }
}

int pingSonar() {
    const unsigned long SonarUpdateInterval = 50;
    static unsigned long prevSonarUpdate = 0;
    static int distance = 200;
    if ((millis() - prevSonarUpdate) >= SonarUpdateInterval) {
        distance = SONAR.ping_cm();
        prevSonarUpdate = millis();
    }
    return distance;
}

bool pingIR() {
    const unsigned long IRUpdateInterval = 50;
    static unsigned long prevIRUpdate = 0;
    static bool breakBeam = false;
    if ((millis() - prevIRUpdate) >= IRUpdateInterval) {
        prevIRUpdate = millis();

        tone(IR_SEND, 38000);
        delay(10);

        int receiveReading = digitalRead(IR_RECEIVE);
        noTone(IR_SEND);

        if (receiveReading == HIGH) {
            breakBeam = true; //Beam Break
        } else {
            breakBeam = false;
        }
    }
    return breakBeam; // No Beam Break
}


void Buzz(bool startBuzz) {
    unsigned long buzzDuration = 300;
    int buzzCooldown = 1000;
    static bool isBuzzing = false;
    static unsigned long prevBuzz = 0;
    

    if (startBuzz && !isBuzzing && (millis() - prevBuzz >= buzzDuration + buzzCooldown)) {
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
    static int debouncedState = HIGH;
    static unsigned long lastDebounce = 0;

    int reading = digitalRead(pin);

    if (reading != prevState) { 
        lastDebounce = millis();
    }

    if ((millis() - lastDebounce) > debounceDelay) {
        debouncedState = reading; 
    }
    
    prevState = reading;
    return (debouncedState == LOW); 
}

bool sendReminder(void *argument) {
    if (WiFi.status() == WL_CONNECTED) {

        JsonDocument doc;
        String outputJson;
        doc["content"] = "Ur Missing Stuff!";
        serializeJson(doc, outputJson);

        WiFiClientSecure client;
        client.setInsecure();
        HTTPClient https;   
        https.begin(client, discordWebhook);

        https.addHeader("Content-Type", "application/json");
        int responseCode = https.POST(outputJson);

        if (responseCode > 0) {
            if (responseCode == HTTP_CODE_OK) {
                Serial.println("message sent");
            } else {
                Serial.println("message error");
            }
        }
        https.end();
        
    }
    return false;
}
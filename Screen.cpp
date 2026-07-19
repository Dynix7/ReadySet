#include "Screen.hpp"
#include "config.cpp"

ScreenHelper::ScreenHelper(Adafruit_SSD1306 *pScreen, struct Status *pcurrentStatus) : 
    Screen(pScreen), currentStatus(pcurrentStatus) {}

void ScreenHelper::initScreen() {
    Screen->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    Screen->clearDisplay();

    Screen->setTextSize(2);
    Screen->setTextColor(SSD1306_WHITE);
    Screen->print("Init :D");

    Screen->display();
    Screen->setTextSize(1);
    lastRefresh = millis();
}

void ScreenHelper:updateScreen() {

    if ((millis() - lastRefresh) > SCREEN_REFRESH) {
        lastRefresh = millis();
        Screen->clearDisplay();

        Screen->setCursor(leftAlign, row0);
        Screen->print("Main Weather: ")
        Screen->println(currentStatus->mainWeather);

        Screen->setCursor(leftAlign, row1);
        Screen->print("Description: ")
        Screen->println(currentStatus->description);

        Screen->setCursor(leftAlign, row2);
        Screen->print("Missing Anything?: ")
        Screen->println(currentStatus->missingStuff);

        //Finish with whatever slop later

        Screen->display();
    }
}
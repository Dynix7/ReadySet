#pragma once
#include <Adafruit_SSD1306.h>
#include "config.hpp"


class ScreenHelper {
    private:
        Adafruit_SSD1306 *Screen = nullptr;
        struct Status *currentStatus = nullptr;

        unsigned long lastRefresh = 0;
        unsigned long deltaTime = 0;

        
        int leftAlign = SCREEN_WIDTH/5;
        int row0 = 0;
        int row1 = 8;
        int row2 = 16;
        int row3 = 24;
        int row4 = 32;
        int row5 = 40;
        int row6 = 48;
        int row7 = 56;

    public:
        ScreenHelper(Adafruit_SSD1306 *pScreen, struct Status *pcurrentStatus);

        void initScreen();

        void updateScreen();

}
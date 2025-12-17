#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <i2c_bus.h>
#include <Beacon.h>


#define DISPLAY_WIDTH   480
#define DISPLAY_HEIGHT  320

#define ROUND_RECT_RADIUS 6

class Display
{
    public :
        enum class TouchType {NONE,PRESS,HOLD,RELEASE};
        Display();
        void bootScreen();
        void setup(I2CBus *i2c);
        void clearDisplay();
        void setReverse(bool reverse);
        bool getReverse();
        int getWidth();
        int getHeight();
        void backlightOn();
        void backlightOff();
        void screenOn();
        void screenOff();
        bool isScreenOn();
        void startDisplayTask();

        // For screenshot
        void screenshot(uint8_t *buffer);

        // Called by lvgl task to peform needed ui updates
        void updateUi();

        // Update ui methods exposed to main loop
        void updateTime() { needUpdateTime = true; };
        void updateLedSig1(bool ledSig1) 
        {
            needUpdateLedSig1 = true;
            ledSig1State = ledSig1;
        }
        void updateLedSig2(bool ledSig2)
        {
            needUpdateLedSig2 = true;
            ledSig2State = ledSig2;
        }
        void updateLedInFrame(bool ledInFrame, bool ledInFrameError) 
        {
            needUpdateLedInFrame = true;
            ledInFrameState = ledInFrame;
            ledInFrameErrorState = ledInFrameError;
        }
        void updateLedFrameReceived(bool ledFrameReceived) 
        {
            needUpdateLedReceived = true;
            ledFrameReceivedState = ledFrameReceived;
        }
        void updatePower() { needUpdatePower = true; }
        void updateFrameReceived (bool state) 
        {
            needUpdateFrameReceived = true;
            frameReceivedState = state;
        }
        void updateAudioPower() { needUpdateAudioPower = true; }
        void updateBeacon(Beacon* beacon, int currentPage, int totalPage)
        {
            needUpdateBeacon = true;
            currentBeacon = beacon;
            currentBeaconPage = currentPage;
            totalBeaconPage = totalPage;
        }
        void updateWifi() { needUpdateWifi = true; }
        void updateSdCard() { needUpdateSdCard = true; }
        void updateDiscri() { needUpdateDiscri = true; }
        void updateTicker() { needUpdateTicker = true; }
        void setScreenOn(bool newScreenOnState) 
        { 
            screenOnState = newScreenOnState;
            needUpdateScreenOn = true; 
        }
        void setScreenReverse(bool newScreenReverseState) 
        { 
            screenReverseState = newScreenReverseState;
            needUpdateScreenReverse = true; 
        }
        void updateSettings() { needUpdateSettings = true; }


    private : 
        int x, y;
        bool reverse = false;
        int touchX = 0, touchY = 0;
        TouchType touchType = TouchType::NONE;
        unsigned long lastTouchReadTime = 0;

        // Update ui flags
        // Time
        bool needUpdateTime = false;
        // Leds
        bool needUpdateLedSig1 = false;
        bool ledSig1State = false;
        bool needUpdateLedSig2 = false;
        bool ledSig2State = false;
        bool needUpdateLedInFrame = false;
        bool ledInFrameState = false;
        bool ledInFrameErrorState = false;
        bool needUpdateLedReceived = false;
        bool ledFrameReceivedState = false;
        // Power
        bool needUpdatePower = false;
        // Frame received
        bool needUpdateFrameReceived = false;
        bool frameReceivedState = false;
        // Audio Power
        bool needUpdateAudioPower = false;
        // Beacon
        bool needUpdateBeacon = false;
        Beacon* currentBeacon = nullptr;
        int currentBeaconPage = 0;
        int totalBeaconPage = 0;
        // Wifi
        bool needUpdateWifi = false;
        // SD
        bool needUpdateSdCard = false;
        // Discri
        bool needUpdateDiscri = false;
        // Ticker
        bool needUpdateTicker = false;
        // Screen on
        bool needUpdateScreenOn = false;
        bool screenOnState = true;
        // Screen reverse
        bool needUpdateScreenReverse = false;
        bool screenReverseState = false;
        // Settings
        bool needUpdateSettings = false;

};

#endif // DISPLAY_H
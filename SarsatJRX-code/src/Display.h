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
        class Color
        {
            public :
            byte red = 0;
            byte green = 0;
            byte blue = 0;
            Color(byte red, byte green, byte blue);
            static const Color White, Black, Red, LightGreen, Green, DarkGreen, Blue, Yellow, Magenta, Beige, Grey, DarkGrey, LightBlue, Purple, Orange;
        };
        enum class TouchType {NONE,PRESS,HOLD,RELEASE};
        Display();
        void setup(I2CBus *i2c);
        void clearDisplay();
        void setReverse(bool reverse);
        bool getReverse();
        void setCursor(int x, int y);
        void setColor(Color color);
        void setBackgroundColor(Color bgColor);
        void setTextColor(Color color);
        void setTextColors(Color color, Color bgColor);
        void fillRectangle(int width, int height);
        void drawRectangle(int width, int height);
        void fillRoundRectangle(int width, int height);
        void drawRoundRectangle(int width, int height);
        void fillCircle(int width, int height);
        void drawCircle(int width, int height);
        void drawLine(int x1,int y1,int x2,int y2);
        /*!
        @param  oradius Outer radius of arc
        @param  iradius Inner radius of arc
        @param  start   degree of arc start
        @param  end     degree of arc end
        */
        void fillArc(int oradius, int iradius, float start, float end);
        void println(String s);
        void println();
        void print(String s);
        void print(long value);
        void printHex(byte value);
        int getWidth();
        int getHeight();
        void backlightOn();
        void backlightOff();
        void setBrightness(uint8_t level);
        void startDisplayTask();

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
            ledFrameReceivedState = state;
        }
        void updateAudioPower() { needUpdateAudioPower = true; }
        void updateBeacon(Beacon* beacon, int currentPage, int totalPage)
        {
            needUpdateBeacon = true;
            currentBeacon = beacon;
            currentBeaconPage = currentPage;
            totalBeaconPage = totalPage;
        }
        void updateWifi()
        {
            needUpdateWifi = true;
        }
        void updateSdCard()
        {
            needUpdateSdCard = true;
        }


    private : 
        int x, y;
        bool reverse = false;
        String displayBuffer;
        Color currentColor = Color::White;
        Color currentBackColor = Color::White;
        Color currentTextColor = Color::White;
        Color currentTextBackColor = Color::Black;
        int touchX = 0, touchY = 0;
        TouchType touchType = TouchType::NONE;
        unsigned long lastTouchReadTime = 0;
        class Colors {
            public:
            const Color* foreground;
            const Color* background;
            const Color* border;
        };

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




};

#endif // DISPLAY_H
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <UTFT.h>
#include <UTouch.h>


class Display
{
    private : 
        // Init display and touch screen
        UTFT myGLCD = UTFT(ITDB32S, 38,39,40,41); 
        UTouch  myTouch = UTouch(6,5,4,3,2);
        int x, y;
        String displayBuffer;

    public :
        Display();
        void setup();
        void clearDisplay();
        void setCursor(int x, int y);
        void println(String s);
        void println();
        void print(String s);
        void print(long value);
        void printHex(byte value);
        void loop();
        void waitForIt(int x1, int y1, int x2, int y2);
        bool touchAvailable();
        int getTouchX();
        int getTouchY();

        void drawButtons();

};

#endif // DISPLAY_H
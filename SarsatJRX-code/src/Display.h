#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <UTFT.h>
#include <UTouch.h>

#define BUTTON_WIDTH  70
#define BUTTON_HEIGHT 28

class Display
{
    public :
        enum class FontSize {SMALL,LARGE};
        class Color
        {
            public :
            byte red = 0;
            byte green = 0;
            byte blue = 0;
            Color(byte red, byte green, byte blue);
            static const Color WHITE, BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN;
        };
        Display();
        void setup();
        void clearDisplay();
        void setCursor(int x, int y);
        void setColor(Color color);
        void setBackgroundColor(Color color);
        void fillRectangle(int width, int height);
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
        int getWidth();
        int getHeight();
        void drawButton(const char* caption, bool pressed);
        void setFontSize(FontSize fontSize);
        void drawButtons();

    private : 
        // Init display and touch screen
        UTFT myGLCD = UTFT(ITDB32S, 38,39,40,41); 
        UTouch  myTouch = UTouch(6,5,4,3,2);
        int x, y;
        String displayBuffer;
        Color currentColor = Color::WHITE;
        Color currentBackColor = Color::BLACK;
        int touchX, touchY;

};

#endif // DISPLAY_H
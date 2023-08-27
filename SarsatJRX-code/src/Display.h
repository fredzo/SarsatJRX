#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <LCDWIKI_TOUCH.h> //touch screen library
#include <qrcode.h>

#define DISPLAY_WIDTH   800
#define DISPLAY_HEIGHT  480

#define BUTTON_WIDTH  140
#define BUTTON_HEIGHT 80
#define SMALL_BUTTON_WIDTH  100
#define SMALL_BUTTON_HEIGHT 80

#define LED_RADIUS 12

#define ROUND_RECT_RADIUS 8

class Display
{
    public :
        enum class FontSize {SMALL,MEDIUM,LARGE};
        enum class ButtonStyle {NORMAL,SMALL};
        class Color
        {
            public :
            byte red = 0;
            byte green = 0;
            byte blue = 0;
            Color(byte red, byte green, byte blue);
            static const Color WHITE, BLACK, RED, LIGHT_GREEN, GREEN, DARK_GREEN, BLUE, YELLOW, MAGENTA, BEIGE, GREY, DARK_GREY, LIGHT_BLUE, PURPLE, ORANGE;
        };
        class Button
        {
            public:
            int x;
            int y;
            const char* caption;
            ButtonStyle style;
            bool enabled = false;
            bool pressed = false;
            Button(int x,int y,const char* caption, ButtonStyle style) : x(x), y(y), caption(caption), style(style) {}
            bool contains(int xPos, int yPos);
            int getWidth();
            int getHeight();
        };
        enum class LedColor {RED,GREEN,BLUE};
        class Led
        {
            public:
            int x;
            int y;
            bool on = false;
            LedColor color;
            Led(int x,int y,LedColor color) : x(x), y(y), color(color) {}
        };
        Display();
        void setup();
        void clearDisplay();
        void setCursor(int x, int y);
        void setColor(Color color);
        void setBackgroundColor(Color color);
        void fillRectangle(int width, int height);
        void drawRectangle(int width, int height);
        void fillRoundRectangle(int width, int height);
        void drawRoundRectangle(int width, int height);
        void fillCircle(int width, int height);
        void drawCircle(int width, int height);
        void println(String s);
        void println();
        void print(String s);
        void print(long value);
        void printHex(byte value);
        bool touchAvailable();
        int getTouchX();
        int getTouchY();
        int getWidth();
        int getHeight();
        void drawButton(Button button);
        void drawLed(Led led);
        void setFontSize(FontSize fontSize);
        void centerText(String text, int width);
        void drawQrCode (QRCode* qrcode,int moduleSize);
        int getFontWidth(FontSize fontSize);
        int getFontHeight(FontSize fontSize);

    private : 
        // Init display and touch screen
        LCDWIKI_KBV myGLCD = LCDWIKI_KBV(NT35510,40,38,39,43,41); //model,cs,cd,wr,rd,reset
        LCDWIKI_TOUCH myTouch = LCDWIKI_TOUCH(53,52,50,51,44); //tcs,tclk,tdout,tdin,tirq
        int x, y;
        String displayBuffer;
        Color currentColor = Color::WHITE;
        Color currentBackColor = Color::BLACK;
        int touchX, touchY;
        FontSize fontSize = FontSize::SMALL;
        class Colors {
            public:
            const Color* foreground;
            const Color* background;
            const Color* border;
        };
        Colors getColorsForButton(Button button);
        Colors getColorsForLed(Led led);

};

#endif // DISPLAY_H
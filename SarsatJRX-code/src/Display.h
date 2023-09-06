#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <qrcode.h>
#include <i2c_bus.h>

#define DISPLAY_WIDTH   480
#define DISPLAY_HEIGHT  320

#define BUTTON_WIDTH  100
#define BUTTON_HEIGHT 50
#define SMALL_BUTTON_WIDTH  80
#define SMALL_BUTTON_HEIGHT 45

#define LED_RADIUS 8

#define ROUND_RECT_RADIUS 6

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
            static const Color White, Black, Red, LightGreen, Green, DarkGreen, Blue, Yellow, Magenta, Beige, Grey, DarkGrey, LightBlue, Purple, Orange;
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
        enum class LedColor {Red,Green,Blue};
        class Led
        {
            public:
            int x;
            int y;
            bool on = false;
            LedColor color;
            Led(int x,int y,LedColor color) : x(x), y(y), color(color) {}
        };
        enum class TouchType {NONE,PRESS,HOLD,RELEASE};
        Display();
        void setup(Color bgColor, I2CBus *i2c);
        void clearDisplay(bool noHeader, bool noFooter);
        void setHeaderAndFooter(int headerHeight, int footerHeight);
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
        TouchType touchAvailable();
        int getTouchX();
        int getTouchY();
        int getWidth();
        int getHeight();
        void drawButton(Button button);
        void drawLed(Led led);
        void setFontSize(FontSize fontSize);
        void centerText(String text);
        void drawQrCode (QRCode* qrcode,int moduleSize);
        int getFontWidth(FontSize fontSize);
        int getFontHeight(FontSize fontSize);
        void backlightOn();
        void backlightOff();
        void setBrightness(uint8_t level);
        void  handleTimer();


    private : 
        int x, y, headerHeight, footerHeight;
        String displayBuffer;
        Color currentColor = Color::White;
        Color currentBackColor = Color::White;
        Color currentTextColor = Color::White;
        Color currentTextBackColor = Color::Black;
        int touchX = 0, touchY = 0;
        TouchType touchType = TouchType::NONE;
        unsigned long lastTouchReadTime = 0;
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
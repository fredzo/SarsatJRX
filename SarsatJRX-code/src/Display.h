#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <UTFT.h>
#include <UTouch.h>

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240

#define BUTTON_WIDTH  70
#define BUTTON_HEIGHT 40
#define SMALL_BUTTON_WIDTH  50
#define SMALL_BUTTON_HEIGHT 40

class Display
{
    public :
        enum class FontSize {SMALL,LARGE};
        enum class ButtonStatus {NORMAL,PRESSED,DISABLED};
        enum class ButtonStyle {NORMAL,SMALL};
        class Color
        {
            public :
            byte red = 0;
            byte green = 0;
            byte blue = 0;
            Color(byte red, byte green, byte blue);
            static const Color WHITE, BLACK, RED, GREEN, DARK_GREEN, BLUE, YELLOW, MAGENTA, CYAN, BEIGE, GREY, DARK_GREY, LIGHT_BLUE, PURPLE, ORANGE;
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
        void setFontSize(FontSize fontSize);
        void centerText(String text, int width);

    private : 
        // Init display and touch screen
        UTFT myGLCD = UTFT(ITDB32S, 38,39,40,41); 
        UTouch  myTouch = UTouch(6,5,4,3,2);
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

};

#endif // DISPLAY_H
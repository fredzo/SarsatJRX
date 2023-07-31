#include <Display.h>

// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t SmallFont[];

Display::Display()
{ 
    x = 0;
    y = 0;
    displayBuffer = "";
}

void Display::setup()
{
    // Initial setup
    myGLCD.InitLCD();
    myGLCD.clrScr();

    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);

    // myGLCD.setFont(BigFont);
    myGLCD.setFont(SmallFont);
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.setColor(255, 255, 255);
}

void Display::setCursor(int x, int y)
{
    Display::x = x;
    Display::y = y;
}

void Display::println(String s)
{   
    displayBuffer+=s;
    // Font adaptation for ° sign
    displayBuffer.replace("°","^");
    // Actually display the string
    myGLCD.print(displayBuffer, x, y);
    displayBuffer = "";
}

void Display::println()
{
    myGLCD.print(displayBuffer, x, y);
    displayBuffer = "";
}

void Display::print(String s)
{
    displayBuffer += s;
}

void Display::print(long value)
{
    displayBuffer += value;
}

void Display::printHex(byte value)
{
    char buffer[4];
    sprintf(buffer, "%02X", value);
    displayBuffer += buffer;
}

void Display::clearDisplay()
{
    myGLCD.clrScr();
    displayBuffer = "";
}

boolean Display::touchAvailable()
{
    if (myTouch.dataAvailable())
    {
        myTouch.read();
        return true;
    }
    else
    {
        return false;
    }
}

int Display::getTouchX()
{
    return myTouch.getX();
}

int Display::getTouchY()
{
    return myTouch.getY();
}

/*************************
**   Custom functions   **
*************************/

void Display::drawButtons()
{
// Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.printNumI(x+1, 27+(x*60), 27);
  }
// Draw the center row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 70, 60+(x*60), 120);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 70, 60+(x*60), 120);
    if (x<4)
      myGLCD.printNumI(x+6, 27+(x*60), 87);
  }
  myGLCD.print("0", 267, 87);
// Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (10, 130, 150, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 150, 180);
  myGLCD.print("Clearo", 40, 147);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (160, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (160, 130, 300, 180);
  myGLCD.print("Entero", 190, 147);
  myGLCD.setBackColor (0, 0, 0);
}

// Draw a red frame while a button is touched
void Display::waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

void Display::loop()
{
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
    }
}

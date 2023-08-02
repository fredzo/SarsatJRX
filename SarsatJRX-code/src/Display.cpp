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

Display::Color::Color(byte red, byte green, byte blue)
{
    this->red = red;
    this->green= green;
    this->blue = blue;
}

const Display::Color Display::Color::WHITE(255,255,255);
const Display::Color Display::Color::BLACK(0,0,0);
const Display::Color Display::Color::RED(206,132,85);
const Display::Color Display::Color::GREEN(106,138,54);
const Display::Color Display::Color::BLUE(28,143,255);
const Display::Color Display::Color::YELLOW(255,255,0);
const Display::Color Display::Color::MAGENTA(255,0,255);
const Display::Color Display::Color::BEIGE(220,220,170);
const Display::Color Display::Color::GREY(121,121,121);
const Display::Color Display::Color::DARK_GREY(24,24,24);
const Display::Color Display::Color::LIGHT_BLUE(156,220,254);
const Display::Color Display::Color::ORANGE(255,200,20);
const Display::Color Display::Color::PURPLE(218,85,131);

void Display::setColor(Color color)
{
  currentColor = color;
  myGLCD.setColor(color.red, color.green, color.blue);
}

void Display::setBackgroundColor(Color color)
{
  currentBackColor = color;
  myGLCD.setBackColor(color.red, color.green, color.blue);
}

void Display::fillRectangle(int width, int height)
{
  myGLCD.fillRect(x,y,x+width,y+height);
}

void Display::drawRectangle(int width, int height)
{
  myGLCD.drawRect(x,y,x+width,y+height);
}

void Display::fillRoundRectangle(int width, int height)
{
  myGLCD.fillRoundRect(x,y,x+width,y+height);
}

void Display::drawRoundRectangle(int width, int height)
{
  myGLCD.drawRoundRect(x,y,x+width,y+height);
}

void Display::setup()
{
    // Initial setup
    myGLCD.InitLCD();
    myGLCD.clrScr();

    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);

    setFontSize(FontSize::SMALL);
    setColor(Color::BLACK);
    setBackgroundColor(Color::WHITE);
}

void Display::setFontSize(FontSize fontSize)
{
  this->fontSize = fontSize;
  switch(fontSize)
  {
    case FontSize::LARGE :
      myGLCD.setFont(BigFont);
      break;
    case FontSize::SMALL :
      myGLCD.setFont(SmallFont);
      break;  
  }
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
    myGLCD.fillScr(myGLCD.getBackColor());
    displayBuffer = "";
}

int Display::getWidth()
{
  return myGLCD.getDisplayXSize();
}

int Display::getHeight()
{
  return myGLCD.getDisplayYSize();
}

boolean Display::touchAvailable()
{
    if (myTouch.dataAvailable())
    {
        myTouch.read();
        touchX = myTouch.getX();
        touchY = myTouch.getY();
        return true;
    }
    else
    {
        return false;
    }
}

int Display::getTouchX()
{
    return touchX;
}

int Display::getTouchY()
{
    return touchY;
}

void Display::centerText(String text, int width)
{
  x += (width-(((fontSize == FontSize::SMALL) ? 8 : 16)*text.length()))/2;
}

void Display::drawButton(const char* caption, bool pressed)
{ // Caption will be up to 6 char long
  int captionSize = strlen(caption);
  // Small font is 8x12
  // Large font is 16x16
  // Store current color
  Color backupColor = currentColor;
  Color backupBackColor = currentBackColor;
  Color fColor = pressed ? Color::YELLOW : Color::BLACK;
  Color bColor = pressed ? Color::BLACK : Color::YELLOW;
  setBackgroundColor(bColor);
  setColor(bColor);
  myGLCD.fillRoundRect (x, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT);
  setColor(fColor);
  myGLCD.print(caption, x+((BUTTON_WIDTH-8*captionSize)/2), y+((BUTTON_HEIGHT-12)/2));
  setColor(Color::ORANGE);
  myGLCD.drawRoundRect (x, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT);
  // Restore color
  setColor(backupColor);
  setBackgroundColor(backupBackColor);
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

#include <Display.h>


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
const Display::Color Display::Color::DARK_GREEN(106,138,54);
const Display::Color Display::Color::GREEN(106,153,85);
const Display::Color Display::Color::LIGHT_GREEN(59,201,176);
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
  myGLCD.Set_Draw_color(color.red, color.green, color.blue);
  myGLCD.Set_Text_colour(color.red, color.green, color.blue);
}

void Display::setBackgroundColor(Color color)
{
  currentBackColor = color;
  myGLCD.Set_Text_Back_colour(color.red, color.green, color.blue);
}

void Display::fillRectangle(int width, int height)
{
  myGLCD.Fill_Rectangle(x,y,x+width,y+height);
}

void Display::drawRectangle(int width, int height)
{
  myGLCD.Draw_Rectangle(x,y,x+width,y+height);
}

void Display::fillRoundRectangle(int width, int height)
{
  myGLCD.Fill_Round_Rectangle(x,y,x+width,y+height,ROUND_RECT_RADIUS);
}

void Display::drawRoundRectangle(int width, int height)
{
  myGLCD.Draw_Round_Rectangle(x,y,x+width,y+height,ROUND_RECT_RADIUS);
}

void Display::setup()
{
    // Initial setup
  myGLCD.Init_LCD();
  myGLCD.Set_Rotation(1);
  myTouch.TP_Set_Rotation(3);
  myTouch.TP_Init(myGLCD.Get_Rotation(),myGLCD.Get_Display_Width(),myGLCD.Get_Display_Height()); 
  myGLCD.Fill_Screen(0,0,0);

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
      myGLCD.Set_Text_Size(4);
      break;
    case FontSize::MEDIUM :
      myGLCD.Set_Text_Size(3);
      break;
    case FontSize::SMALL :
      myGLCD.Set_Text_Size(2);
      break;  
  }
}

int Display::getFontWidth(FontSize fontSize)
{
  switch(fontSize)
  {
    case FontSize::LARGE :
      return 28; // 4*7
    case FontSize::MEDIUM :
      return 21; // 3*7
    case FontSize::SMALL :
      return 14; // 2*7
  }
}

int Display::getFontHeight(FontSize fontSize)
{
  switch(fontSize)
  {
    case FontSize::LARGE :
      return 64; // 4*16
    case FontSize::MEDIUM :
      return 48; // 3*16
    case FontSize::SMALL :
      return 32; // 2*16
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
    myGLCD.Print_String(displayBuffer, x, y);
    displayBuffer = "";
}

void Display::println()
{
    myGLCD.Print_String(displayBuffer, x, y);
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
    myGLCD.Fill_Screen(0,0,0);
    displayBuffer = "";
}

int Display::getWidth()
{
  return DISPLAY_WIDTH;
}

int Display::getHeight()
{
  return DISPLAY_HEIGHT;
}

boolean Display::touchAvailable()
{
    myTouch.TP_Scan(0);
    if (myTouch.TP_Get_State()&TP_PRES_DOWN)
    {
        touchX = myTouch.x;
        touchY = myTouch.y;
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
  x += (width-((getFontWidth(fontSize))*text.length()))/2;
}

int Display::Button::getWidth()
{
  switch (style)
  {
    case ButtonStyle::SMALL:
      return SMALL_BUTTON_WIDTH;
      break;
    case ButtonStyle::NORMAL:
    default:
      return BUTTON_WIDTH;
      break;
  }
}

int Display::Button::getHeight()
{
  switch (style)
  {
    case ButtonStyle::SMALL:
      return SMALL_BUTTON_HEIGHT;
      break;
    case ButtonStyle::NORMAL:
    default:
      return BUTTON_HEIGHT;
      break;
  }
}

bool Display::Button::contains(int xPos, int yPos)
{
  
  return (xPos >= x && xPos <= (x+getWidth()) && yPos >= y && yPos <= (y+getHeight()));
}

Display::Colors Display::getColorsForButton(Display::Button button)
{
  Colors result;
  switch (button.style)
  {
    case ButtonStyle::SMALL :
      result.foreground = &(button.pressed ? Color::GREY : button.enabled ? Color::LIGHT_BLUE : Color::GREY);
      result.background = &(button.pressed ? Color::LIGHT_BLUE : button.enabled ? Color::GREY : Color::DARK_GREY);
      result.border = &Color::PURPLE;
      break;
    case ButtonStyle::NORMAL :
    default :
      result.foreground = &(button.pressed ? Color::YELLOW : button.enabled ? Color::BLACK :Color::BEIGE);
      result.background = &(button.pressed ? Color::BLACK : button.enabled ? Color::YELLOW : Color::GREY);
      result.border = &Color::ORANGE;
      break;
  }
  return result;
}

Display::Colors Display::getColorsForLed(Display::Led led)
{
  Colors result;
  switch (led.color)
  {
    case LedColor::RED :
      result.foreground = &(led.on ? Color::RED : Color::GREY);
      result.border = &Color::ORANGE;
      break;
    case LedColor::BLUE :
      result.foreground = &(led.on ? Color::BLUE : Color::GREY);
      result.border = &Color::LIGHT_BLUE;
      break;
    case LedColor::GREEN :
    default :
      result.foreground = &(led.on ? Color::GREEN : Color::GREY);
      result.border = &Color::LIGHT_GREEN;
      break;
  }
  return result;
}

void Display::drawButton(Button button)
{ 
  int captionSize = strlen(button.caption);
  // Store current color
  Colors colors = getColorsForButton(button);
  Color backupColor = currentColor;
  Color backupBackColor = currentBackColor;
  // Store font size
  FontSize backupFontSize = fontSize;
  setFontSize(FontSize::MEDIUM);
  int xx = button.x+button.getWidth();
  int yy = button.y+button.getHeight();
  setBackgroundColor(*colors.background);
  setColor(*colors.background);
  myGLCD.Fill_Round_Rectangle(button.x, button.y, xx, yy, ROUND_RECT_RADIUS);
  setColor(*colors.foreground);
  myGLCD.Print_String(button.caption, button.x+((button.getWidth()-16*captionSize)/2), button.y+((button.getHeight()-16)/2));
  setColor(*colors.border);
  myGLCD.Draw_Round_Rectangle(button.x, button.y, xx, yy, ROUND_RECT_RADIUS);
  // Restore color
  setColor(backupColor);
  setBackgroundColor(backupBackColor);
  // Restore font size
  fontSize = backupFontSize;
}

void Display::drawLed(Led led)
{ 
  // Store current color
  Colors colors = getColorsForLed(led);
  Color backupColor = currentColor;
  setColor(*colors.foreground);
  myGLCD.Fill_Circle(led.x, led.y,LED_RADIUS);
  setColor(*colors.border);
  myGLCD.Draw_Circle(led.x, led.y,LED_RADIUS);
  // Restore color
  setColor(backupColor);
}

void Display::drawQrCode (QRCode* qrcode, int moduleSize)
{
  int xPos = x;
  int yPos = y;
  setColor(Display::Color::WHITE);
  // Add a padding around the QR code
  fillRectangle((qrcode->size+2)*moduleSize,(qrcode->size+2)*moduleSize);
  setColor(Display::Color::BLACK);
  xPos += moduleSize;
  yPos += moduleSize;
  for (int iY = 0; iY < qrcode->size; iY++) 
  {
      for (int iX = 0; iX < qrcode->size; iX++) 
      {
          if (qrcode_getModule(qrcode, iX, iY)) 
          {
              x = xPos + (iX * moduleSize);
              y = yPos + (iY * moduleSize);
							fillRectangle(moduleSize, moduleSize);
          }
      }
  }
}
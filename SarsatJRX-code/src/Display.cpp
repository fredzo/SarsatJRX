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

const Display::Color Display::Color::White(255,255,255);
const Display::Color Display::Color::Black(0,0,0);
const Display::Color Display::Color::Red(206,132,85);
const Display::Color Display::Color::DarkGreen(106,138,54);
const Display::Color Display::Color::Green(106,153,85);
const Display::Color Display::Color::LightGreen(59,201,176);
const Display::Color Display::Color::Blue(28,143,255);
const Display::Color Display::Color::Yellow(255,255,0);
const Display::Color Display::Color::Magenta(255,0,255);
const Display::Color Display::Color::Beige(220,220,170);
const Display::Color Display::Color::Grey(121,121,121);
const Display::Color Display::Color::DarkGrey(10,10,10);
const Display::Color Display::Color::LightBlue(156,220,254);
const Display::Color Display::Color::Orange(255,200,20);
const Display::Color Display::Color::Purple(218,85,131);

void Display::setColor(Color color)
{
  currentColor = color;
}

void Display::setBackgroundColor(Color bgColor)
{
  currentBackColor = bgColor;
}

void Display::setTextColor(Color color)
{
  currentTextColor = color;
  myGLCD->setTextColor(RGB565(currentTextColor.red, currentTextColor.green, currentTextColor.blue));
}

void Display::setTextColors(Color color, Color bgColor)
{
  currentTextColor = color;
  currentTextBackColor = bgColor;
  myGLCD->setTextColor(RGB565(currentTextColor.red, currentTextColor.green, currentTextColor.blue),RGB565(currentTextBackColor.red, currentTextBackColor.green, currentTextBackColor.blue));
}

void Display::fillRectangle(int width, int height)
{
  myGLCD->fillRect(x,y,width,height,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::drawRectangle(int width, int height)
{
  myGLCD->drawRect(x,y,width,height,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::fillRoundRectangle(int width, int height)
{
  myGLCD->fillRoundRect(x,y,width,height,ROUND_RECT_RADIUS,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::drawRoundRectangle(int width, int height)
{
  myGLCD->drawRoundRect(x,y,width,height,ROUND_RECT_RADIUS,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::setup(Color bgColor)
{
  // Initial setup
  myGLCD->begin();
  //myGLCD.Set_Rotation(1);
  myTouch.TP_Set_Rotation(3);
  myTouch.TP_Init(1,DISPLAY_WIDTH,DISPLAY_HEIGHT); 
  fontSize = FontSize::SMALL;
  currentBackColor = bgColor;
  currentColor = Color::White;
  currentTextBackColor = bgColor;
  currentTextColor = Color::White;
  clearDisplay(false);
}

void Display::setFontSize(FontSize fontSize)
{
  this->fontSize = fontSize;
  switch(fontSize)
  {
    case FontSize::LARGE :
      myGLCD->setTextSize(4);
      break;
    case FontSize::MEDIUM :
      myGLCD->setTextSize(3);
      break;
    case FontSize::SMALL :
      myGLCD->setTextSize(2);
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
  return 21;
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
  return 48;
}

void Display::setCursor(int x, int y)
{
    Display::x = x;
    Display::y = y;
    myGLCD->setCursor(x,y);
}

void Display::setHeaderHeight(int headerHeight)
{
    Display::headerHeight = headerHeight;
}

void Display::println(String s)
{   
    displayBuffer+=s;
    // Font adaptation for ° sign
    displayBuffer.replace("°","^");
    // Actually display the string
    myGLCD->println(displayBuffer);
    displayBuffer = "";
}

void Display::println()
{
    myGLCD->println(displayBuffer);
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

void Display::clearDisplay(bool noHeader)
{
  if(noHeader)
  {
    myGLCD->fillRect(0,headerHeight,DISPLAY_WIDTH,DISPLAY_HEIGHT,RGB565(currentBackColor.red, currentBackColor.green, currentBackColor.blue));
  }
  else
  {
    myGLCD->fillScreen(RGB565(currentBackColor.red, currentBackColor.green, currentBackColor.blue));
  }
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
  myGLCD->setCursor(x,y);
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
      result.foreground = &(button.pressed ? Color::Grey : button.enabled ? Color::LightBlue : Color::Grey);
      result.background = &(button.pressed ? Color::LightBlue : button.enabled ? Color::Grey : Color::DarkGrey);
      result.border = &Color::Purple;
      break;
    case ButtonStyle::NORMAL :
    default :
      result.foreground = &(button.pressed ? Color::Yellow : button.enabled ? Color::Black :Color::Beige);
      result.background = &(button.pressed ? Color::Black : button.enabled ? Color::Yellow : Color::Grey);
      result.border = &Color::Orange;
      break;
  }
  return result;
}

Display::Colors Display::getColorsForLed(Display::Led led)
{
  Colors result;
  switch (led.color)
  {
    case LedColor::Red :
      result.foreground = &(led.on ? Color::Red : Color::Grey);
      result.border = &Color::Orange;
      break;
    case LedColor::Blue :
      result.foreground = &(led.on ? Color::Blue : Color::Grey);
      result.border = &Color::LightBlue;
      break;
    case LedColor::Green :
    default :
      result.foreground = &(led.on ? Color::Green : Color::Grey);
      result.border = &Color::LightGreen;
      break;
  }
  return result;
}

void Display::drawButton(Button button)
{ 
  int captionSize = strlen(button.caption);
  // Store current color
  Colors colors = getColorsForButton(button);
  Color backupTextColor = currentTextColor;
  Color backupTextBackColor = currentTextBackColor;
  // Store font size
  FontSize backupFontSize = fontSize;
  setFontSize(FontSize::MEDIUM);
  setTextColors(*colors.foreground,*colors.background);
  Color color = *colors.background;
  myGLCD->fillRoundRect(button.x, button.y, button.getWidth(), button.getHeight(), ROUND_RECT_RADIUS,RGB565(color.red, color.green, color.blue));
  color = *colors.foreground;
  myGLCD->setCursor(button.x+((button.getWidth()-16*captionSize)/2), button.y+((button.getHeight()-16)/2));
  myGLCD->println(button.caption);
  color = *colors.border;
  myGLCD->drawRoundRect(button.x, button.y, button.getWidth(), button.getHeight(), ROUND_RECT_RADIUS,RGB565(color.red, color.green, color.blue));
  // Restore text colors
  setTextColors(backupTextColor,backupTextBackColor);
  // Restore font size
  fontSize = backupFontSize;
}

void Display::drawLed(Led led)
{ 
  Colors colors = getColorsForLed(led);
  Color color = *colors.foreground;
  myGLCD->fillCircle(led.x, led.y,LED_RADIUS,RGB565(color.red, color.green, color.blue));
  color = *colors.border;
  myGLCD->drawCircle(led.x, led.y,LED_RADIUS,RGB565(color.red, color.green, color.blue));
}

void Display::drawQrCode (QRCode* qrcode, int moduleSize)
{
  int xPos = x;
  int yPos = y;
  setColor(Display::Color::White);
  // Add a padding around the QR code
  fillRectangle((qrcode->size+2)*moduleSize,(qrcode->size+2)*moduleSize);
  setColor(Display::Color::Black);
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
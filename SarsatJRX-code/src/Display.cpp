#include <Display.h>
#include <touch.h>
//#include <lvgl.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

#define LILYPI_TFT_MISO            23
#define LILYPI_TFT_MOSI            19
#define LILYPI_TFT_SCLK            18
#define LILYPI_TFT_CS              5
#define LILYPI_TFT_DC              27
#define LILYPI_TFT_RST             GFX_NOT_DEFINED
#define LILYPI_TFT_BL              12


// Init display and touch screen
Arduino_DataBus *bus = new Arduino_ESP32SPI(LILYPI_TFT_DC, LILYPI_TFT_CS, LILYPI_TFT_SCLK, LILYPI_TFT_MOSI, LILYPI_TFT_MISO);
Arduino_GFX *myGLCD = new Arduino_ILI9481_18bit(bus, GFX_NOT_DEFINED, 1 /* rotation */, false /* IPS */);


// For LVGL ///////////////////////////////////////////////////////////////////////

/* Display flushing */
/*void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  myGLCD->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}*/

//////////////////////////////////////////////////////////////////////////////////

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

void Display::fillArc(int oradius, int iradius, float start, float end)
{
  myGLCD->fillArc(x,y,oradius,iradius,start,end,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::drawRoundRectangle(int width, int height)
{
  myGLCD->drawRoundRect(x,y,width,height,ROUND_RECT_RADIUS,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::drawLine(int x1,int y1,int x2,int y2)
{
  myGLCD->drawLine(x1,y1,x2,y2,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::setup(Color bgColor)
{
  // Initial setup
  myGLCD->begin();
  touch_init(DISPLAY_WIDTH,DISPLAY_HEIGHT,1);
  setFontSize(FontSize::SMALL);
  currentBackColor = bgColor;
  currentColor = Color::White;
  currentTextBackColor = bgColor;
  currentTextColor = Color::White;
  clearDisplay(false,false);
}

void Display::setFontSize(FontSize fontSize)
{
  this->fontSize = fontSize;
  switch(fontSize)
  {
    case FontSize::LARGE :
      //myGLCD->setTextSize(4); // 24x32
      myGLCD->setFont(u8g2_font_logisoso32_tf);
      break;
    case FontSize::MEDIUM :
      //myGLCD->setTextSize(3); // 18x24
      myGLCD->setFont( u8g2_font_inr24_mf );
      break;
    case FontSize::SMALL :
      //myGLCD->setTextSize(2); // 12x16
      myGLCD->setFont(u8g2_font_inr16_mf);
      break;  // 1 = 6x8
  }
}

int Display::getFontWidth(FontSize fontSize)
{
  switch(fontSize)
  {
    case FontSize::LARGE :
      return 24; // 4*6
    case FontSize::MEDIUM :
      return 18; // 3*6
    case FontSize::SMALL :
      return 12; // 2*6
  }
  return 18;
}

int Display::getFontHeight(FontSize fontSize)
{
  switch(fontSize)
  {
    case FontSize::LARGE :
      return 32; // 4*8
    case FontSize::MEDIUM :
      return 24; // 3*8
    case FontSize::SMALL :
      return 16; // 2*8
  }
  return 24;
}

void Display::setCursor(int x, int y)
{
    Display::x = x;
    Display::y = y;
    myGLCD->setCursor(x,y);
}

void Display::setHeaderAndFooter(int headerHeight, int footerHeight)
{
    Display::headerHeight = headerHeight;
    Display::footerHeight = footerHeight;
}

void Display::println(String s)
{   
    displayBuffer+=s;
    // Font adaptation for ° sign
    displayBuffer.replace("°","\xB0");

    // u8g2 fonts are printend with bottom left position instead of top left...
    setCursor(x,y+getFontHeight(fontSize));
    // Actually display the string
    myGLCD->println(displayBuffer);
    // Restpre original position
    setCursor(x,y);
    displayBuffer = "";
}

void Display::println()
{
    Display::println("");
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

void Display::clearDisplay(bool noHeader, bool noFooter)
{
  if(noHeader)
  {
    int heigth = DISPLAY_HEIGHT;
    if(noHeader) heigth-=headerHeight;
    if(noFooter) heigth-=footerHeight;
    myGLCD->fillRect(0,noHeader ? headerHeight : 0,DISPLAY_WIDTH,heigth,RGB565(currentBackColor.red, currentBackColor.green, currentBackColor.blue));
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
    if(touch_touched())
    {
      #ifdef TOUCH_CAL
        touchX = touch_raw_x;
        touchY = touch_raw_y;
      #else
        touchX = touch_last_x;
        touchY = touch_last_y;
      #endif
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

void Display::centerText(String text)
{
  x -= ((getFontWidth(fontSize))*text.length())/2;
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
  int margin = (style == ButtonStyle::SMALL) ? 80 : 5; // Use large margin of error for small buttons to compensate resistive screen innacurracy
  return (xPos >= x-margin && xPos <= (x+getWidth()+margin) && yPos >= y-margin && yPos <= (y+getHeight()+margin));
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
  FontSize buttonFontSize = (button.style == ButtonStyle::SMALL) ? FontSize::MEDIUM : FontSize::SMALL;
  setFontSize(buttonFontSize);
  setTextColors(*colors.foreground,*colors.background);
  Color color = *colors.background;
  myGLCD->fillRoundRect(button.x, button.y, button.getWidth(), button.getHeight(), ROUND_RECT_RADIUS,RGB565(color.red, color.green, color.blue));
  color = *colors.foreground;
  myGLCD->setCursor(button.x+((button.getWidth()-getFontWidth(buttonFontSize)*captionSize)/2), button.y+button.getHeight()-((button.getHeight()-getFontHeight(buttonFontSize))/2));
  myGLCD->println(button.caption);
  color = *colors.border;
  myGLCD->drawRoundRect(button.x, button.y, button.getWidth(), button.getHeight(), ROUND_RECT_RADIUS,RGB565(color.red, color.green, color.blue));
  // Restore text colors
  setTextColors(backupTextColor,backupTextBackColor);
  // Restore font size
  setFontSize(backupFontSize);
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
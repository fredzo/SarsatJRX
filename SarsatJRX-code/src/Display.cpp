#include <Display.h>
#include <lvgl.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <gt911.h>
#include <ledc.h>
#include <Free_Fonts.h>



#define LILYPI_TFT_MISO             23
#define LILYPI_TFT_MOSI             19
#define LILYPI_TFT_SCLK             18
#define LILYPI_TFT_CS               5
#define LILYPI_TFT_DC               27
#define LILYPI_TFT_RST              -1
#define LILYPI_TFT_BL               12
#define GT911_ADDRESS               0x5D

#define TFT_ESPI_DRIVER             0x9481
#define TFT_ESPI_FREQ               27000000

#define RGB565(r, g, b) ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3))

// Init display and touch screen
TFT_eSPI *myGLCD = new TFT_eSPI(DISPLAY_HEIGHT,DISPLAY_WIDTH);
BackLight *bl = new BackLight(LILYPI_TFT_BL);
GT9xx_Class *touch = new GT9xx_Class();

// For LVGL ///////////////////////////////////////////////////////////////////////

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ DISPLAY_WIDTH * DISPLAY_HEIGHT / 10 ];

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    myGLCD->setAddrWindow( area->x1, area->y1, w, h );
    myGLCD->pushColors( ( uint16_t * )&color_p->full, w * h, true );

    lv_disp_flush_ready(disp_drv);
}

void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    bool touched = (touch->scanPoint() > 0);
    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(touched)
    {
      uint16_t x = 0,y = 0;
      touch->getPoint(x,y,0);
      // Handle rotation (cf. TTGO.h l. 299)
      data->point.x = DISPLAY_WIDTH - y;
      data->point.y = x;
      /*Serial.print( "Data x " );
      Serial.println( x );
      Serial.print( "Data y " );
      Serial.println( y );*/
    }
}


//////////////////////////////////////////////////////////////////////////////////

Display::Display()
{ 
    x = 0;
    y = 0;
    displayBuffer = "";
}

void Display::setup(I2CBus *i2c)
{
  // Lvgl init
  lv_init();
  // Initial setup
  myGLCD->init();
  myGLCD->setRotation(3);
  currentColor = Color::White;
  currentTextColor = Color::White;
  //clearDisplay();
  // Init touch screen
  if (!touch->begin(Wire, GT911_ADDRESS)) {
      Serial.println("Begin touch FAIL");
  }
  else
  {
      Serial.println("Begin touch OK !");
  }
  // Init backlight
  bl->begin();
  bl->on();
  
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, DISPLAY_WIDTH * DISPLAY_HEIGHT / 10 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  disp_drv.hor_res = DISPLAY_WIDTH;
  disp_drv.ver_res = DISPLAY_HEIGHT;
  disp_drv.flush_cb = disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_drv);
}

void Display::handleTimer()
{
    lv_timer_handler(); /* let the GUI do its work */
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
  myGLCD->drawArc(x,y,oradius,iradius,start,end,RGB565(currentColor.red, currentColor.green, currentColor.blue),RGB565(currentBackColor.red, currentBackColor.green, currentBackColor.blue),true);
}

void Display::drawRoundRectangle(int width, int height)
{
  myGLCD->drawRoundRect(x,y,width,height,ROUND_RECT_RADIUS,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::drawLine(int x1,int y1,int x2,int y2)
{
  myGLCD->drawLine(x1,y1,x2,y2,RGB565(currentColor.red, currentColor.green, currentColor.blue));
}

void Display::setCursor(int x, int y)
{
    Display::x = x;
    Display::y = y;
    myGLCD->setCursor(x,y);
}

void Display::println(String s)
{   
    displayBuffer+=s;
    // Font adaptation for ° sign
    //displayBuffer.replace("°","\xB0");
    displayBuffer.replace("°","'");

    // u8g2 fonts are printend with bottom left position instead of top left...
    //setCursor(x,y+getFontHeight(fontSize));
    // Actually display the string
    myGLCD->drawString(displayBuffer,x,y);
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

void Display::clearDisplay()
{
  myGLCD->fillScreen(RGB565(currentBackColor.red, currentBackColor.green, currentBackColor.blue));
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

#define FOOTER_READ_PERIOD 50

Display::TouchType Display::touchAvailable()
{
  unsigned long now = millis();
  if((now - lastTouchReadTime) < FOOTER_READ_PERIOD)
  {
    return touchType;
  }
  lastTouchReadTime = now;
  uint16_t x = 0,y = 0;
  if (touch->scanPoint() > 0)
  {
    touch->getPoint(x,y,0);
    // Handle rotation (cf. TTGO.h l. 299)
    touchX = DISPLAY_WIDTH - y;
    touchY = x;
    if(touchType == TouchType::NONE)
    {
      touchType = TouchType::PRESS;
      //Serial.println("Press");
    }
    else
    {
      touchType = TouchType::HOLD;
      //Serial.println("Hold");
    }
    return touchType;
  }
  else
  {
    if(touchType == TouchType::PRESS || touchType == TouchType::HOLD)
    {
      touchType = TouchType::RELEASE;
      //Serial.println("Release");
    }
    else if(touchType != TouchType::NONE)
    {
      touchType = TouchType::NONE;
      //Serial.println("NONE");
    }
    return touchType;
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

/******************************************
 *              BACKLIGHT
 * ***************************************/
void Display::backlightOn()
{
    bl->on();
}

void Display::backlightOff()
{
    bl->off();
}

void Display::setBrightness(uint8_t level)
{
    bl->adjust(level);
}

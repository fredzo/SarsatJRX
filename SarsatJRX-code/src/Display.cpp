#include <Display.h>
#include <SarsatJRXConf.h>
#include <lvgl.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <gt911.h>
#include <ledc.h>
#include <ui/Ui.h>
#include <ui/UiSettings.h>
#include <SoundManager.h>
#include <Settings.h>


#define GT911_ADDRESS               0x5D

#define TFT_ESPI_DRIVER             0x9481
#define TFT_ESPI_FREQ               27000000

#define RGB565(r, g, b) ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3))

// Init display and touch screen
TFT_eSPI *myGLCD = new TFT_eSPI(DISPLAY_HEIGHT,DISPLAY_WIDTH);
GT9xx_Class *touch = new GT9xx_Class();
bool reverseScreen = false;

// Scren / backlight management
static bool screenIsOn = true;

static void blOn()
{
  digitalWrite(LILYPI_TFT_BL,1);
}

static void blOff()
{
  digitalWrite(LILYPI_TFT_BL,0);
}

static void scrOn()
{
  if(screenIsOn) return;
  screenIsOn = true;
  blOn();
  // Force redraw all screen
  lv_obj_invalidate(lv_scr_act()); 
}

static void scrOff()
{
  if(!screenIsOn) return;
  screenIsOn = false;
  blOff();
  myGLCD->fillScreen(RGB565(0, 0, 0));
}


// For LVGL ///////////////////////////////////////////////////////////////////////

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ DISPLAY_WIDTH * DISPLAY_HEIGHT / 10 ];

// LVGL dedicated task
void lvglTask(void *pvParameters) 
{
  Display* display = (Display*)pvParameters;
  while (1) {
    // First update ui elements according to display class state
    display->updateUi();
    // Then hand it over to lvgl
    lv_timer_handler();  // Handles animations, redraw, input
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{   
    if(screenIsOn)
    { // No display update if screen is off
      uint32_t w = ( area->x2 - area->x1 + 1 );
      uint32_t h = ( area->y2 - area->y1 + 1 );

      myGLCD->setAddrWindow( area->x1, area->y1, w, h );
      myGLCD->pushColors( ( uint16_t * )&color_p->full, w * h, true );
    }

    lv_disp_flush_ready(disp_drv);
}

static bool wasTouched = false;

void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    bool touched = (touch->scanPoint() > 0);
    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(touched)
    {
      if(!screenIsOn)
      { // Wake up display if needed
        scrOn();
      }

      uint16_t x = 0,y = 0;
      touch->getPoint(x,y,0);
      // Handle rotation (cf. TTGO.h l. 299)
      if(reverseScreen)
      {
        data->point.x = DISPLAY_WIDTH - y;
        data->point.y = x;
      }
      else
      {
        data->point.x = y;
        data->point.y = DISPLAY_HEIGHT - x;
      }
      /*Serial.print( "Data x " );
      Serial.println( x );
      Serial.print( "Data y " );
      Serial.println( y );*/
      if(!wasTouched && Settings::getSettings()->getTouchSound())
      {
        SoundManager::getSoundManager()->playTouchSound();
        wasTouched = true;
      }
    }
    else
    {
      wasTouched = false;
    }
}


//////////////////////////////////////////////////////////////////////////////////

Display::Display()
{ 
    x = 0;
    y = 0;
}

void Display::updateUi()
{ // Checkf for requested udpates
  if(needUpdateLedSig1)         uiSetLedSig1State(ledSig1State);
  needUpdateLedSig1 = false;
  if(needUpdateLedSig2)         uiSetLedSig2State(ledSig2State);
  needUpdateLedSig2 = false;
  if(needUpdateLedInFrame)      uiSetLedInFrameState(ledInFrameState,ledInFrameErrorState);
  needUpdateLedInFrame = false;
  if(needUpdateLedReceived)     uiSetLedFrameReceivedState(ledFrameReceivedState);
  needUpdateLedReceived = false;
  if(needUpdateTime)            uiUpdateTime();
  needUpdateTime = false;
  if(needUpdatePower)           uiUpdatePower();
  needUpdatePower = false;
  if(needUpdateFrameReceived)   uiShowFrameReceived(frameReceivedState);
  needUpdateFrameReceived = false;
  if(needUpdateAudioPower)      uiUpdateAudioPower();
  needUpdateAudioPower = false;
  if(needUpdateBeacon)          uiSetBeacon(currentBeacon,currentBeaconPage,totalBeaconPage);
  needUpdateBeacon = false;
  if(needUpdateWifi)            uiUpdateWifiStatus();
  needUpdateWifi = false;
  if(needUpdateSdCard)          uiUpdateSdCardStatus();
  needUpdateSdCard = false;
  if(needUpdateDiscri)          uiUpdateDiscritatus();
  needUpdateDiscri = false;
  if(needUpdateTicker)          uiUpdateTicker();
  needUpdateTicker = false;
  if(needUpdateScreenOn)        screenOnState ? screenOn() : screenOff();
  needUpdateScreenOn = false;
  if(needUpdateScreenReverse)   setReverse(screenReverseState);
  needUpdateScreenReverse = false;
  if(needUpdateSettings)        uiSettingsUpdateView();
  needUpdateSettings = false;
}

extern uint8_t boot_logo_map[];

void Display::bootScreen()
{ 
  // Init TFT_eSPI driver
  myGLCD->init();
  myGLCD->setRotation(1);
  // Push boot logo
  myGLCD->pushImage(0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT,boot_logo_map);
  blOn();
}

void Display::setup(I2CBus *i2c)
{
  // Lvgl init
  lv_init();
  // Initial setup
  reverseScreen = reverse;
  // Init touch screen
  if (!touch->begin(Wire, GT911_ADDRESS)) {
      Serial.println("Begin touch FAIL");
  }
  else
  {
      Serial.println("Begin touch OK !");
  }
  
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

  // Build ui
  createUi();
  uiUpdateTime();
  uiUpdatePower();
  updateLedSig1(false);
  updateLedSig2(false);
  updateLedInFrame(false,false);
  updateLedFrameReceived(false);
}

void Display::setReverse(bool reverse)
{
  if(reverse != this->reverse)
  {
    this->reverse = reverse;
    myGLCD->setRotation(reverse ? 3 : 1);
    reverseScreen = reverse;
    // Force redraw all screen
    lv_obj_invalidate(lv_scr_act()); 
  }
}

bool Display::getReverse()
{
  return reverse;
}

void Display::startDisplayTask()
{
  // Create LVGL task on Core 1
  xTaskCreatePinnedToCore(
    lvglTask,      // Task fubction
    "LVGL Task",   // Task name
    2*4096,          // Stack size
    this,          // Params
    configMAX_PRIORITIES,// 1,             // Priority
    NULL,          // Handle
    1              // Core 1
  );
}

int Display::getWidth()
{
  return DISPLAY_WIDTH;
}

int Display::getHeight()
{
  return DISPLAY_HEIGHT;
}

void Display::screenOn()
{
  scrOn();
}

void Display::screenOff()
{
  scrOff();
}

bool Display::isScreenOn() 
{
  return screenIsOn; 
}


/******************************************
 *              BACKLIGHT
 * ***************************************/
void Display::backlightOn()
{
  blOn();
}

void Display::backlightOff()
{
  blOff();
}
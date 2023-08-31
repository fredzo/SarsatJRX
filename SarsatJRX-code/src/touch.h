/*******************************************************************************
 * Touch libraries:
 * XPT2046: https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
 *
 * Capacitive touchscreen libraries
 * TouchLib: https://github.com/mmMicky/TouchLib.git
 ******************************************************************************/
#include <Arduino.h>

// Please fill below values from Arduino_GFX Example - TouchCalibration
bool touch_swap_xy = true;
int16_t touch_map_x1 = 3980;
int16_t touch_map_x2 = 60;
int16_t touch_map_y1 = 200;
int16_t touch_map_y2 = 3900;

int16_t touch_max_x = 0, touch_max_y = 0;
int16_t touch_raw_x = 0, touch_raw_y = 0;
uint16_t touch_last_x = 0, touch_last_y = 0;

#include <Wire.h>
#include <gt911.h>
GT9xx_Class touch;


void touch_init(int16_t w, int16_t h, uint8_t r)
{
  touch_max_x = w - 1;
  touch_max_y = h - 1;

  touch.begin();
}

bool touch_touched()
{

  if (touch.scanPoint())
  {
    touch.getPoint(touch_last_x,touch_last_y,0);
    return true;
  }


  return false;
}

bool touch_released()
{
  return false;
}

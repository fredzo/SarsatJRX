#include "Power.h"
#include <SarsatJRXConf.h>

uint16_t adc_vref = 1100;

/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float getPowerVccValue() {
    uint16_t v = analogRead(BATTERY_ADC_PIN);
    return  ((float)v / 4095.0) * 2.0 * 3.3 * (adc_vref / 1000.0);
}

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @return The VCC string value
 */
String getPowerVccStringValue()
{   /* 6 is mininum width, 4 is precision; float value is copied onto angleStr */
    char buffer[8];
    dtostrf(getPowerVccValue(), 3, 2, buffer);
    return (String(buffer) + "V");
}


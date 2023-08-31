#include "Power.h"

#define ADC_PIN             35

uint16_t adc_vref = 1100;

/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float getVccValue() {
        uint16_t v = analogRead(ADC_PIN);
        return  ((float)v / 4095.0) * 2.0 * 3.3 * (adc_vref / 1000.0);
}

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @param vccString the char* to store the VCC string value in
 */
void getVccStringValue(char* vccString)
{   /* 6 is mininum width, 4 is precision; float value is copied onto angleStr */
    dtostrf(getVccValue(), 3, 2, vccString);
}


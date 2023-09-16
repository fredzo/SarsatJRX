#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float getPowerVccValue();

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @return Rhe VCC string value
 */
String getPowerVccStringValue();

#endif 
#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float getVccValue();

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @param vccString the char* to store the VCC string value in
 */
void getVccStringValue(char* vccString);

#endif 
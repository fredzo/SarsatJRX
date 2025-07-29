#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

typedef enum {
    POWER_STATE_ON_BATTERY,
    POWER_STATE_CHARGING,
    POWER_STATE_FULL
} PowerState;

/**
 * @brief Init power module
 * 
 */
void powerInit();

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

/**
 * @brief Get current PowerState
 * 
 * @return PowerState  the current PowerState
 */
PowerState getPowerState();

#endif 
#include "Power.h"
#include <SarsatJRXConf.h>

#define MAX_BATTERY_VOLTAGE     4.2


// Static members
Power *Power::powerInstance = nullptr;


void Power::powerInit()
{
  pinMode(BATTERY_ADC_PIN, ANALOG);      // Battery level pin
  pinMode(CHARGE_INPUT_PIN, INPUT);      // Charge status pin
}


/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float Power::getVccValue() {
    uint32_t v = analogReadMilliVolts(BATTERY_ADC_PIN);
    return  (((float)v)*2.0/1000.0);
}

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @return The VCC string value
 */
String Power::getVccStringValue()
{   /* 6 is mininum width, 4 is precision; float value is copied onto angleStr */
    char buffer[8];
    dtostrf(getVccValue(), 3, 2, buffer);
    return (String(buffer) + "V");
}

Power::PowerState Power::getPowerState()
{
    PowerState result;
    if(digitalRead(CHARGE_INPUT_PIN))
    {   // Led off (pin is high) => Either on battery or full
        if(getVccValue() >= MAX_BATTERY_VOLTAGE)
        {
            result = PowerState::POWER_STATE_FULL;
        }
        else
        {
            result = PowerState::POWER_STATE_ON_BATTERY;
        }
    }
    else
    {   // Charging
        result = PowerState::POWER_STATE_CHARGING;
    }
    return result;
}



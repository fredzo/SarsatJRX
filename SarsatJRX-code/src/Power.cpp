#include "Power.h"
#include <SarsatJRXConf.h>

#define MAX_BATTERY_VOLTAGE     4.19
#define POWER_SAMPLE_PERIOD     1000 // 1s
#define CHARGE_SAMPLE_PERIOD    50   // 50ms
#define CHARGE_SAMPLE_COUNT     15   // Wait for 15 consecutive samples with same value to validate charge state


// Static members
Power *Power::powerInstance = nullptr;


void Power::powerInit()
{
  pinMode(BATTERY_ADC_PIN, ANALOG);      // Battery level pin
  pinMode(CHARGE_INPUT_PIN, INPUT_PULLUP);      // Charge status pin
}


/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float Power::getVccValue() {
    changed = false;
    return powerValue;
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
    return state;
}

String Power::getPowerStateString()
{
    String result;
    switch (getPowerState())
    {
    case PowerState::FULL :
        result = "Battery full";
        break;
    case PowerState::ON_BATTERY :
        result = "On Battery";
        break;
    case PowerState::NO_BATTERY :
        result = "Battery switch off";
        break;
    case PowerState::CHARGING :
    default:
        result = "Charging";
        break;
    }
    return result;
}

// Power task processing
void Power::handleTask()
{   // This task is in charge of updating power value and power state
    unsigned long now = millis();
    if(now - lastPowerSampleTime > POWER_SAMPLE_PERIOD)
    {   // Time to make a new sample
        lastPowerSampleTime = now;
        uint32_t v = analogReadMilliVolts(BATTERY_ADC_PIN);
        float newValue = (((float)v)*2.0/1000.0);
        if(abs(newValue - powerValue) > 0.01)
        {   // Filter small value changes
            powerValue = newValue;
            changed = true;
        }
    }
    if(now - lastChargeSampleTime > CHARGE_SAMPLE_PERIOD)
    {   // Time to make a new sample
        lastChargeSampleTime = now;
        int newValue = digitalRead(CHARGE_INPUT_PIN);
        if(newValue == lastChargeValue)
        {   // Is pin value stable (we want to detect the fickering state of the charging led of the TP4054 that refects no battery is connected (battery switch is off))
            if((chargeValueCount < CHARGE_SAMPLE_COUNT) || changed) // If power value changed, we may need to update state too
            {   // Count consecutive values
                chargeValueCount++;
                if(chargeValueCount >= CHARGE_SAMPLE_COUNT)
                {   // Update power state if value is stable
                    PowerState newState;
                    if(lastChargeValue)
                    {   // Led off (pin high) => either full or on battery
                        if(powerValue >= MAX_BATTERY_VOLTAGE)
                        {
                            newState = PowerState::FULL;
                        }
                        else
                        {
                            newState = PowerState::ON_BATTERY;
                        }
                    }
                    else
                    {   // Charging
                        newState = PowerState::CHARGING;
                    }
                    if(newState != state)
                    {
                        state = newState;
                        changed = true;
                    }
                }
            }
        }
        else
        {   // Led is flickering => no battery connected
            chargeValueCount = 0;
            if(state != PowerState::NO_BATTERY)
            {
                state = PowerState::NO_BATTERY;
                changed = true;
            }
            lastChargeValue = newValue;
        }
    }
}





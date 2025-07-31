#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

class Power
{
    public :
        enum class PowerState { ON_BATTERY, CHARGING, FULL, NO_BATTERY };

        static Power *getPower()
        {
            if (powerInstance == nullptr) {
                powerInstance = new Power();
            }
            return powerInstance;
        }

        void powerInit();
        void powerStop();
        
        /**
         * @brief Returns actual VCC value
         * 
         * @return VCC value
         */
        float getVccValue();

        /**
         * @brief Read VCC value into the provided char* with the format "x.xx"
         * 
         * @return Rhe VCC string value
         */
        String getVccStringValue();

        /**
         * @brief Get current PowerState
         * 
         * @return PowerState  the current PowerState
         */
        PowerState getPowerState();

        /**
         * @brief Get the Power State String representation
         * 
         * @return String the Power State String representation
         */
        String getPowerStateString();

        /**
         * @brief Returns true if power has changed since last call to getVccValue()
         * 
         * @return true if power has changed since last call to getVccValue()
         */
        bool hasChanged() { return changed; };

        // Power task processing
        void handleTask();

    private :
        Power()
        {
        };

        ~Power()
        {
        };
        // Current power state
        PowerState state = PowerState::ON_BATTERY;
        // Current vcc value
        float powerValue = 0;
        // Last power sample time
        unsigned long lastPowerSampleTime = 0;
        // Last charge sample time
        unsigned long lastChargeSampleTime = 0;
        // True when power changed
        bool changed = true;
        // Last value of charge pin
        int lastChargeValue = 0;
        // Charge value count
        int chargeValueCount = 0;

        // Static members
        static Power *powerInstance;
};
#endif 
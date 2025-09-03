#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

#define BATTERY_BUFFER_SIZE     128  // Circular buffer for rolling average

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
         * @brief Returns raw VCC value
         * 
         * @return raw VCC value
         */
        float getRawVccValue() { return rawPowerValue; };

        /**
         * @brief Read raw VCC value into the provided char* with the format "x.xx"
         * 
         * @return The raw VCC string value
         */
        String getRawVccStringValue();

        /**
         * @brief Read VCC value into the provided char* with the format "x.xx"
         * 
         * @return The VCC string value
         */
        String getVccStringValue();

        /**
         * @brief Get the Battery Percentage value
         * 
         * @return int the battery percentage value
         */
        int getBatteryPercentage();

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

        /**
         * @brief Returns true if power state has changed since last call of this method
         * 
         * @return true if power state has changed since last call of this method
         */
        bool hasPowerStateChanged();

        // Power task processing
        void handleTask();

    private :
        Power()
        {
        };

        ~Power()
        {
        };

        void addPowerSample(float newVoltage);
        float getAveragePowerValue();

        // Current power state
        PowerState state = PowerState::ON_BATTERY;
        // Current vcc value
        float powerValue = 0;
        // Raw vcc value
        float rawPowerValue = 0;
        // Current battery percentage
        int batteryPercentage = 0;
        // Last power sample time
        unsigned long lastPowerSampleTime = 0;
        // Last power update time
        unsigned long lastPowerUpdateTime = 0;
        // Last charge sample time
        unsigned long lastChargeSampleTime = 0;
        // True when power changed
        bool changed = true;
        // Last value of charge pin
        int lastChargeValue = 0;
        // Number of charge samples
        uint16_t chargeSampleCount = 0;
        // Charge value count
        int chargeValueCount = 0;
        // True when power state changed
        bool powerStateChanged = false;

        float voltageBuffer[BATTERY_BUFFER_SIZE];
        uint8_t voltageIndex = 0;
        uint8_t voltageCount = 0;
        float voltageSum = 0.0f;

        // Static members
        static Power *powerInstance;
};
#endif 
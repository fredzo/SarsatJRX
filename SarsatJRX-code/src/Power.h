#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

class Power
{
    public :
        enum class PowerState { POWER_STATE_ON_BATTERY, POWER_STATE_CHARGING, POWER_STATE_FULL };

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

        // Power task processing
        void handleTask();

    private :
        Power()
        {
        };

        ~Power()
        {
        };

        // Static members
        static Power *powerInstance;
};
#endif 
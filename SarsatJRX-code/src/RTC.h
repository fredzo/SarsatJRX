#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <i2c_bus.h>
#include <pcf8563.h>


class Rtc
{
    public :
        static Rtc *getRtc()
        {
            if (rtcInstance == nullptr) {
                rtcInstance = new Rtc();
            }
            return rtcInstance;
        }

        class Date
        {
            public :
                int day;
                int month;
                int year;
                int hour;
                int minute;
                int second;
        };
        
        void rtcInit(I2CBus* i2c);
        void changeTime() {changed = true;};
        Date getDate();
        String getDateString();
        String getTimeString();
        bool hasChanged() {return changed;};

    private :
        I2CBus *rtcI2c = nullptr; 
        PCF8563_Class *rtc = nullptr;
        void rtcRegisterInterrupt();
        bool changed;
        Date currentDate;

        Rtc()
        {
        };

        ~Rtc()
        {
        };

        static Rtc *rtcInstance;
};

#endif 
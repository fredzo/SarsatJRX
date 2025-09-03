#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <i2c_bus.h>
#include <pcf8563.h>

#define RTC_MAX_COUNTDOWN   999

#define RTC_FRAME_COUNTDOWN 50


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
                String getDateString();
                String getTimeString();
        };
        
        void rtcInit(I2CBus* i2c);
        void changeTime();
        void tickSecond();
        Date getDate();
        void setDate(Date date);
        String getDateString();
        String getTimeString();
        String getUptimeString();
        bool hasChanged()   { return changed; };
        bool isNtpSynched() { return ntpSynched; };
        int  getCountDown();
        bool countDownHasChanged();
        bool isLastCount();
        bool isAlmostLastCount();
        void setCountDown(int value);
        void startCountDown() { setCountDown(RTC_FRAME_COUNTDOWN); };
        void countDown();

    private :
        I2CBus *rtcI2c = nullptr; 
        PCF8563_Class *rtc = nullptr;
        void rtcRegisterInterrupt();
        void setSystemTime(RTC_Date* dt);
        bool changed = true;
        bool ntpStarted = false;
        bool ntpSynched = false;
        Date currentDate;
        int countDownValue = 0;
        bool countDownChanged = false;
        hw_timer_t * timer = NULL;

        Rtc()
        {
        };

        ~Rtc()
        {
        };

        static Rtc *rtcInstance;
};

#endif 
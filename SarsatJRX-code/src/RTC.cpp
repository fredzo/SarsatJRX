#include "RTC.h"

#define RTC_INT_PIN         36

Rtc *Rtc::rtcInstance = nullptr;

String Rtc::Date::getDateString()
{
    char buffer[16];
    sprintf(buffer,"%02d/%02d/%04d", day,month,year);
    return String(buffer);
}

String Rtc::Date::getTimeString()
{
    char buffer[16];
    sprintf(buffer,"%02d:%02d:%02d", hour,minute,second);
    return String(buffer);
}


void Rtc::rtcInit(I2CBus* i2c)
{
    rtcI2c = i2c;
    rtc = new PCF8563_Class(*i2c);
    rtcRegisterInterrupt();
    rtc->enableTimer();
    rtc->setTimer(1 /* 1 tick */ ,0b10 /* 1Hz frequecy for timer source */,true /* enable interrupt timer */);
}

Rtc::Date Rtc::getDate()
{
    if(changed)
    {
        RTC_Date dt = rtc->getDateTime();
        currentDate.day = dt.day;
        currentDate.month = dt.month;
        currentDate.year = dt.year;
        currentDate.hour = dt.hour;
        currentDate.minute = dt.minute;
        currentDate.second = dt.second;
        changed = false;
    }
    return currentDate;
}

String Rtc::getDateString()
{
    return getDate().getDateString();
}

String Rtc::getTimeString()
{
    return getDate().getTimeString();
}

void rtcInterruptCallack(void)
{
    Rtc::getRtc()->changeTime();
}

void Rtc::rtcRegisterInterrupt()
{
    pinMode(RTC_INT_PIN, INPUT);
    attachInterrupt(RTC_INT_PIN, rtcInterruptCallack, FALLING);
}


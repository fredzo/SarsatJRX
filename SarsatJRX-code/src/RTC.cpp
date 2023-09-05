#include "RTC.h"

#define RTC_INT_PIN         36

Rtc *Rtc::rtcInstance = nullptr;

void Rtc::rtcInit(I2CBus* i2c)
{
    rtcI2c = i2c;
    rtc = new PCF8563_Class(*i2c);
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
    Date date = getDate();
    char buffer[16];
    sprintf(buffer,"%2d/%2d/%4d", date.day,date.month,date.year);
    return String(buffer);
}

String Rtc::getTimeString()
{
    Date date = getDate();
    char buffer[16];
    sprintf(buffer,"%2d:%2d:%2d", date.hour,date.minute,date.second);
    return String(buffer);
}

void rtcInterruptCallack(void)
{
    Rtc::getRtc()->changeTime();
}

void Rtc::rtcRegisterInterrupt()
{
    pinMode(RTC_INT_PIN, INPUT_PULLUP); //need change to rtc_pin
    attachInterrupt(RTC_INT_PIN, rtcInterruptCallack, FALLING);
}

// TODO

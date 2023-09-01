#include "RTC.h"
#include "pcf8563.h"

#define RTC_INT_PIN         36

I2CBus *i2c = nullptr; 
PCF8563_Class *rtc = nullptr;

void rtcInit()
{
    i2c = new I2CBus();
    rtc = new PCF8563_Class(*i2c);
}

void rtcInterruptCallack(void)
{
    // TODO
}

void rtcInitInterrupt()
{
    pinMode(RTC_INT_PIN, INPUT_PULLUP); //need change to rtc_pin
    attachInterrupt(RTC_INT_PIN, rtcInterruptCallack, FALLING);
}

// TODO

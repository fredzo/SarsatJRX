#include <RTC.h>
#include <SarsatJRXConf.h>
#include <WifiManager.h>
#include <time.h>
#include <esp_sntp.h>

Rtc *Rtc::rtcInstance = nullptr;

String Rtc::Date::getDateString()
{
    char buffer[16];
    snprintf(buffer,sizeof(buffer),"%02d/%02d/%04d", day,month,year);
    return String(buffer);
}

String Rtc::Date::getTimeString()
{
    char buffer[16];
    snprintf(buffer,sizeof(buffer),"%02d:%02d:%02d", hour,minute,second);
    return String(buffer);
}

void IRAM_ATTR onTimer() 
{
    Rtc::getRtc()->tickSecond();
}

void Rtc::rtcInit(I2CBus* i2c)
{
    rtcI2c = i2c;
    rtc = new PCF8563_Class(*i2c);
    rtcRegisterInterrupt();
    rtc->enableTimer();
    rtc->setTimer(1 /* 1 tick */ ,0b10 /* 1Hz frequecy for timer source */,true /* enable interrupt timer */);
    // Setup 1sec tick timer for countDown
    timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz (1 µs)
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000, true); // 1 sec
    timerAlarmEnable(timer);
    // Preset the year to something close to the present
    RTC_Date dt = rtc->getDateTime();
    if(dt.year < 2025)
    {
        dt.year = 2025;
        rtc->setDateTime(dt);
    }
    // Update system time
    setSystemTime(&dt);
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
        #ifdef WIFI
        if(!ntpSynched && wifiManagerIsConnected())
        {   // Try and get time from NTP server
            if(!ntpStarted)
            {   // Start NTP client and specify TZ
                configTzTime(TIME_ZONE,NTP_SERVER);
                ntpStarted = true;
                #ifdef SERIAL_OUT
                Serial.println("NTP Server started !");
                #endif
            }
            struct tm timeinfo;
            if(getLocalTime(&timeinfo,0))
            {   // Update RTC
                dt.day = timeinfo.tm_mday;
                dt.month = timeinfo.tm_mon+1;
                dt.year = timeinfo.tm_year;
                dt.hour = timeinfo.tm_hour;
                dt.minute = timeinfo.tm_min;
                dt.second = timeinfo.tm_sec;
                rtc->setDateTime(dt);
                // Stop ntp service
                sntp_stop();
                ntpSynched = true;
                // Make sure we update time display right away
                changed = true;
                #ifdef SERIAL_OUT
                Serial.println("NTP time received ! :");
                Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
                #endif
            }
        }
        #endif
    }
    return currentDate;
}

void Rtc::setSystemTime(RTC_Date* dt)
{
  // Définir la date et l'heure
  struct tm t;
  t.tm_year = dt->year - 1900;  // Years since 1900
  t.tm_mon  = dt->month -1;     // Month 0-11 (0=Jan)
  t.tm_mday = dt->day;          // Day of month
  t.tm_hour = dt->hour;         // Hour
  t.tm_min  = dt->minute;       // Min
  t.tm_sec  = dt->second;       // Sec
  t.tm_isdst = -1;              // Automatic DST detection

  time_t now = mktime(&t); // Convertir en timestamp
  struct timeval tv = { .tv_sec = now, .tv_usec = 0 };
  settimeofday(&tv, NULL); // Positionner l'heure système
}


void Rtc::setDate(Date date)
{
    RTC_Date dt;
    dt.hour = date.hour;
    dt.minute = date.minute;
    dt.second = date.second;
    dt.day = date.day;
    dt.month = date.month;
    dt.year = date.year;
    rtc->setDateTime(dt);
    changed = true;
    // Update system time
    setSystemTime(&dt);
}

String Rtc::getDateString()
{
    return getDate().getDateString();
}

String Rtc::getTimeString()
{
    return getDate().getTimeString();
}

int Rtc::getCountDown()
{
    return countDownValue;
}

bool Rtc::isAlmostLastCount()
{
    return (countDownValue <= 5 && countDownValue > 1);
}

bool Rtc::isLastCount()
{
    return (countDownValue == 1);
}

void Rtc::setCountDown(int newCountDown)
{
    if(newCountDown < -RTC_MAX_COUNTDOWN)
    {
        countDownValue = 0;
    }
    else if(newCountDown > RTC_MAX_COUNTDOWN)
    {
        countDownValue = RTC_MAX_COUNTDOWN;
    }
    else
    {
        countDownValue = newCountDown;
    }
    countDownChanged = true;
    timerRestart(timer);
}

bool Rtc::countDownHasChanged()
{
    bool result = countDownChanged;
    countDownChanged = false;
    return result;
}

void Rtc::changeTime()
{   // Mark time as changed
    changed = true;
}

void Rtc::tickSecond()
{   // Update countdown
    countDownValue--;
    if(countDownValue < -RTC_MAX_COUNTDOWN)
    {
        countDownValue = 0;
    }
    countDownChanged = true;
}

void IRAM_ATTR rtcInterruptCallack(void)
{
    Rtc::getRtc()->changeTime();
}

void Rtc::rtcRegisterInterrupt()
{
    pinMode(RTC_INT_PIN, INPUT);
    attachInterrupt(RTC_INT_PIN, rtcInterruptCallack, FALLING);
}


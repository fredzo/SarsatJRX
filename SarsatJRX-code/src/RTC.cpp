#include <RTC.h>
#include <SarsatJRXConf.h>
#include <WifiManager.h>
#include <Settings.h>
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

void IRAM_ATTR rtcInterruptCallack(void)
{
    Rtc::getRtc()->changeTime();
}

void IRAM_ATTR onCountdownTimer() 
{
    Rtc::getRtc()->tickSecond();
}

void Rtc::rtcInit(I2CBus* i2c)
{
    rtcI2c = i2c;
    rtc = new PCF8563_Class(*i2c);
    // For some reason, RTC interrup is not working (probably an hardware issue with Lily Pi board...)
    // Let's use an ESP32 hardware timer instead
    //rtcRegisterInterrupt();
    //rtc->setTimer(1 /* 1 tick */ ,0b10 /* 1Hz frequecy for timer source */,true /* enable interrupt timer */);
    //rtc->enableTimer();
    clockTimer = timerBegin(3, 80, true);  // 80 MHz / 80 = 1 MHz (1 µs)
    timerAttachInterrupt(clockTimer, &rtcInterruptCallack, true);
    timerAlarmWrite(clockTimer, 1000000, true); // 1 sec
    timerAlarmEnable(clockTimer);
    // Setup 1sec tick timer for countDown
    countdownTimer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz (1 µs)
    timerAttachInterrupt(countdownTimer, &onCountdownTimer, true);
    timerAlarmWrite(countdownTimer, 1000000, true); // 1 sec
    timerAlarmEnable(countdownTimer);
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
                configTzTime(Settings::getSettings()->getTimeZone().c_str(),NTP_SERVER1,NTP_SERVER2,NTP_SERVER3);
                ntpStarted = true;
                #ifdef SERIAL_OUT
                Serial.println("NTP Server started !");
                #endif
            }
            uint32_t now = millis();
            uint32_t delay = (ntpRequestNumber * NTP_REQUEST_BASE_DELAY);
            if((now - lastNtpRequestTime) >= delay)
            {   // Start with short request delay and increase the delay on each request
                //Serial.printf("Ntp request #%d, delay=%d, actual=%d.\n",ntpRequestNumber,delay,now-lastNtpRequestTime);
                lastNtpRequestTime = now;
                ntpRequestNumber++;
                struct tm timeinfo;
                if(!ntpWait)
                {   // First call => Force local time to 2015 to make sure getLocalTime() waits for NTP to resolve
                    dt.year = 2015;
                    setSystemTime(&dt,false);
                    ntpWait = true;
                }
                if(getLocalTime(&timeinfo,0))
                {   // Update RTC
                    dt.day = timeinfo.tm_mday;
                    dt.month = timeinfo.tm_mon+1;
                    dt.year = timeinfo.tm_year+1900;
                    dt.hour = timeinfo.tm_hour;
                    dt.minute = timeinfo.tm_min;
                    dt.second = timeinfo.tm_sec;
                    rtc->setDateTime(dt);
                    // Stop ntp service
                    sntp_stop();
                    ntpSynched = true;
                    ntpStarted = false;
                    ntpWait = false;
                    ntpRequestNumber = 0;
                    // Make sure we update time display right away
                    changed = true;
                    #ifdef SERIAL_OUT
                    Serial.println("NTP time received ! :");
                    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
                    #endif
                }
                else 
                {
                    Serial.println("Could not get time from NTP server.");
                }
            }
        }
        #endif
    }
    return currentDate;
}

void Rtc::setSystemTime(RTC_Date* dt, bool restartTimer)
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
  if(restartTimer) timerRestart(clockTimer);
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

String Rtc::getUptimeString() 
{
  unsigned long millisecs = millis(); // ms since boot
  
  unsigned long seconds = millisecs / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours   = minutes / 60;
  unsigned long days    = hours / 24;

  seconds %= 60;
  minutes %= 60;
  hours   %= 24;

  // Format
  String uptime;
  if(days > 0) uptime = String(days) + "d";
  if(hours > 0) uptime += (((hours > 9) ? "" : "0") + String(hours) + "h");
  if(minutes > 0) uptime += (((minutes > 9) ? "" : "0") + String(minutes) + "'");
  uptime += (((seconds > 9) ? "" : "0") + String(seconds) + "\"");
  return uptime;
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
    timerRestart(countdownTimer);
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
    {   // -1 instead of 0 to prevent auto-relaod countdown
        countDownValue = -1;
    }
    countDownChanged = true;
}

void Rtc::rtcRegisterInterrupt()
{
    pinMode(RTC_INT_PIN, INPUT_PULLUP);
    attachInterrupt(RTC_INT_PIN, rtcInterruptCallack, FALLING);
}


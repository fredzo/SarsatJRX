#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX         "SarsatJRX"
#define WIFI_STATE_KEY      "Wifi"
#define RADIO_STATE_KEY     "Radio"
#define RADIO_VOLUME_KEY    "Vol"
#define RADIO_AUTO_VOL_KEY  "AutoVol"
#define RADIO_FILTER1_KEY   "Filt1"
#define RADIO_FILTER2_KEY   "Filt2"
#define RADIO_FILTER3_KEY   "Filt3"
#define RADIO_FREQ_T_KEY    "FrqF%d"
#define RADIO_FREQ_ON_T_KEY "FrqO%d"

#define FREQUENCY_COUNT     7
float Settings::DEFAULT_FREQUENCIES[] = {406.025,406.028,406.037,406.040,406.049,406.052,433.125};

Settings *Settings::settingsInstance = nullptr;

Settings::Frequency Settings::NO_FREQUENCY;

void Settings::init()
{
    preferences.begin(PREF_PREFIX, false);
}

bool Settings::getWifiState()
{
    return preferences.getBool(WIFI_STATE_KEY,true);
}

void Settings::setWifiState(bool state)
{
    preferences.putBool(WIFI_STATE_KEY,state);
    dirty = true;
}

bool Settings::getRadioState()
{
    return preferences.getBool(RADIO_STATE_KEY,true);
}

void Settings::setRadioState(bool state)
{
    preferences.putBool(RADIO_STATE_KEY,state);
    dirty = true;
}

bool Settings::getRadioAutoVolume()
{
    return preferences.getBool(RADIO_AUTO_VOL_KEY,true);
}

void Settings::setRadioAutoVolume(bool state)
{
    preferences.putBool(RADIO_AUTO_VOL_KEY,state);
    dirty = true;
}

byte Settings::getRadioVolume()
{
    return preferences.getChar(RADIO_VOLUME_KEY,8);
}

void Settings::setRadioVolume(byte volume)
{
    preferences.putChar(RADIO_VOLUME_KEY,volume);
    dirty = true;
}

bool Settings::getRadioFilter1()
{
    return preferences.getBool(RADIO_FILTER1_KEY,true);
}

void Settings::setRadioFilter1(bool state)
{
    preferences.putBool(RADIO_FILTER1_KEY,state);
    dirty = true;
}

bool Settings::getRadioFilter2()
{
    return preferences.getBool(RADIO_FILTER2_KEY,true);
}

void Settings::setRadioFilter2(bool state)
{
    preferences.putBool(RADIO_FILTER2_KEY,state);
    dirty = true;
}

bool Settings::getRadioFilter3()
{
    return preferences.getBool(RADIO_FILTER3_KEY,true);
}

void Settings::setRadioFilter3(bool state)
{
    preferences.putBool(RADIO_FILTER3_KEY,state);
    dirty = true;
}

Settings::Frequency Settings::getFrequency(int index)
{
    if(index < 0 || index >= FREQUENCY_COUNT) return NO_FREQUENCY;
    Frequency result;
    char buffer[8];
    sprintf(buffer,RADIO_FREQ_T_KEY,index);
    result.value = preferences.getFloat(buffer,DEFAULT_FREQUENCIES[index]);
    sprintf(buffer,RADIO_FREQ_ON_T_KEY,index);
    result.value = preferences.getBool(buffer,true);
    return result;
}

void Settings::setFrequency(int index, Frequency frequency)
{
   if(index < 0 || index >= FREQUENCY_COUNT) return;
    char buffer[8];
    sprintf(buffer,RADIO_FREQ_T_KEY,index);
    preferences.putFloat(buffer,frequency.value);
    sprintf(buffer,RADIO_FREQ_ON_T_KEY,index);
    preferences.putBool(buffer,frequency.on);
}

void Settings::setFrequency(int index, float frequency)
{
    if(index < 0 || index >= FREQUENCY_COUNT) return;
    char buffer[8];
    sprintf(buffer,RADIO_FREQ_T_KEY,index);
    preferences.putFloat(buffer,frequency);
}

void Settings::setFrequencyOn(int index, bool on)
{
    if(index < 0 || index >= FREQUENCY_COUNT) return;
    char buffer[8];
    sprintf(buffer,RADIO_FREQ_ON_T_KEY,index);
    preferences.putBool(buffer,on);
}

int Settings::getFrequencyCount()
{
    return FREQUENCY_COUNT;
}

float* Settings::getActiveFrequencies()
{
    float result[FREQUENCY_COUNT+1];
    int index = 0;
    for(int i = 0 ; i <= FREQUENCY_COUNT ; i++)
    {
        Frequency freq = getFrequency(i);
        if(freq.on)
        {   
            result[index] = freq.value;
            index++;
        }
    }
    result[index] = 0; // Trailing 0 to mark array end
    return result;
}

void Settings::save()
{
    if(dirty)
    {
        /*Serial.println("Saving preferences...");
        Serial.printf("Wifi : %d\n",getWifiState());
        Serial.printf("Radio : %d\n",getRadioState());
        Serial.printf("Filter1 : %d\n",getRadioFilter1());*/
        preferences.end();
        preferences.begin(PREF_PREFIX, false);
        dirty = false;
    }
}

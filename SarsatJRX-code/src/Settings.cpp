#include <Settings.h>
#include <SarsatJRXConf.h>
#include <Filesystems.h>
#include <Util.h>

#define PREF_PREFIX             "SarsatJRX"

Settings *Settings::settingsInstance = nullptr;

static const Settings::Setting BLUETOOTH_STATE      ("BT"   ,"bluetoothState"       ,"Bluetooth on (true/false) ?");
static const Settings::Setting WIFI_STATE           ("Wifi" ,"wifiState"            ,"Wifi on (true/false) ?");
static const Settings::Setting WIFI_SSID            ("SSID" ,"wifiSsid"             ,"Ssid of the wifi network to connect to");
static const Settings::Setting WIFI_PASS_PHRASE     ("PSK"  ,"wifiPassPhrase"       ,"Passphrase of the Wifi network to connect to");
static const Settings::Setting WIFI_SSID1           ("SSID1","wifiSsid1"            ,"Ssid of the 2nd wifi network to connect to");
static const Settings::Setting WIFI_PASS_PHRASE1    ("PSK1" ,"wifiPassPhrase1"      ,"Passphrase of 2nd the Wifi network to connect to");
static const Settings::Setting WIFI_SSID2           ("SSID2","wifiSsid2"            ,"Ssid of the 3rd wifi network to connect to");
static const Settings::Setting WIFI_PASS_PHRASE2    ("PSK2" ,"wifiPassPhrase2"      ,"Passphrase of the 3rd Wifi network to connect to");
static const Settings::Setting TIME_ZONE            ("TZ"   ,"timeZone"             ,"TimeZone (see https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv )");
static const Settings::Setting DISPLAY_REVERSE      ("DRev" ,"displayReverse"       ,"Display reverse (true/false) ?");
static const Settings::Setting SHOW_BAT_PERCENTAGE  ("SBP"  ,"showBatPercentage"    ,"Show battery percentage (true/false) ?");
static const Settings::Setting SHOW_BAT_WARN_MESSAGE("SBW"  ,"showBatWarnMessage"   ,"Show warn message when power cable is plugged but battery is not charging because power switch is off (true/false) ?");
static const Settings::Setting SCREEN_OFF_ON_CHRAGE ("SOOC" ,"screenOffOnCharge"    ,"Turn screen off when charging (true/false) ?");
static const Settings::Setting BUZZER_LEVEL         ("BL"   ,"buzzerLevel"          ,"Buzzer level (0-255) ?");
static const Settings::Setting TOUCH_SOUND          ("TS"   ,"touchSound"           ,"Beep when touching screen (true/false) ?");
static const Settings::Setting FRAME_SOUND          ("FS"   ,"frameSound"           ,"Sound notification when a frame is received (true/false) ?");
static const Settings::Setting COUNTDOWN_SOUND      ("CDS"  ,"countdownSound"       ,"Sound countdown before next frame (true/false) ?");
static const Settings::Setting COUNTDOWN_LEDS       ("CDL"  ,"countdownLeds"        ,"Led countdown before next frame (true/false) ?");
static const Settings::Setting RELOAD_COUNTDOWN     ("RCD"  ,"reloadCountdown"      ,"Automatically reload countdown at the end, even if no frame has been received (true/false) ?");
static const Settings::Setting COUNTDOWN_DURATION   ("CDD"  ,"countdownDuration"    ,"Countdown duration in seconds (0-255) ?");
static const Settings::Setting ALLOW_FRAME_SIMU     ("AFS"  ,"allowFrameSimu"       ,"Allow firing simulation frame when 'SarsatJRX' header is long pressed (true/false) ?");
static const Settings::Setting FILTER_ORBITO        ("FO"   ,"fliterOrbito"         ,"Filter orbitography frames (useful near the CNES at Toulouse) (true/false) ?");
static const Settings::Setting FILTER_INVALID       ("FI"   ,"filterInvalid"        ,"Filter invalid frames (when BCH1/BCH2 control codes are invalid) (true/false) ?");

void Settings::init()
{
    preferences.begin(PREF_PREFIX, false);
    // Load all settings on startup to avoid accessing them at runtime
    bluetoothState = preferences.getBool(BLUETOOTH_STATE.key.c_str(),false);
    wifiState = preferences.getBool(WIFI_STATE.key.c_str(),false);
    wifiSsid = preferences.getString(WIFI_SSID.key.c_str(),"SarsatJRX");
    wifiPassPhrase = preferences.getString(WIFI_PASS_PHRASE.key.c_str(),"SarsatJRX");
    wifiSsid1 = preferences.getString(WIFI_SSID1.key.c_str(),"KrakenAndroid");
    wifiPassPhrase1 = preferences.getString(WIFI_PASS_PHRASE1.key.c_str(),"KrakenAndroid");
    wifiSsid2 = preferences.getString(WIFI_SSID2.key.c_str(),"");
    wifiPassPhrase2 = preferences.getString(WIFI_PASS_PHRASE2.key.c_str(),"");
    timeZone = preferences.getString(TIME_ZONE.key.c_str(),NTP_TIME_ZONE);
    displayReverse = preferences.getBool(DISPLAY_REVERSE.key.c_str(),false);
    screenOffOnCharge = preferences.getBool(SCREEN_OFF_ON_CHRAGE.key.c_str(),true);
    showBatteryPercentage = preferences.getBool(SHOW_BAT_PERCENTAGE.key.c_str(),true);
    showBatteryWarnMessage = preferences.getBool(SHOW_BAT_WARN_MESSAGE.key.c_str(),true);
    buzzerLevel = preferences.getUChar(BUZZER_LEVEL.key.c_str(),120);
    touchSound = preferences.getBool(TOUCH_SOUND.key.c_str(),true);
    frameSound = preferences.getBool(FRAME_SOUND.key.c_str(),true);
    countDownSound = preferences.getBool(COUNTDOWN_SOUND.key.c_str(),true);
    countDownLeds = preferences.getBool(COUNTDOWN_LEDS.key.c_str(),true);
    reloadCountDown = preferences.getBool(RELOAD_COUNTDOWN.key.c_str(),false);
    countdownDuration = preferences.getUChar(COUNTDOWN_DURATION.key.c_str(),FRAME_COUNTDOWN_DEFAULT_DURATION);
    allowFrameSimu = preferences.getBool(ALLOW_FRAME_SIMU.key.c_str(),false);
    filterOrbito = preferences.getBool(FILTER_ORBITO.key.c_str(),false);
    filterInvalid = preferences.getBool(FILTER_INVALID.key.c_str(),true);
    // Check for configuraion file on SD card
    Filesystems* filesystems = Filesystems::getFilesystems();
    if(filesystems->isSdFilesystemMounted())
    {
        std::vector<String> lines;
        if(filesystems->loadConfigFile(lines))
        {   // Parse config lines
            for (auto line : lines) 
            {   
                //Serial.printf("Parsing conf line:%s\n",line.c_str());
                line.trim();
                // Ignore comments
                if(line.charAt(0)!='#')
                {
                    size_t equalIndex = line.indexOf("=");
                    if(equalIndex > 0)
                    {
                        String key = line.substring(0,equalIndex);
                        String value = line.substring(equalIndex+1);
                        setSettingValue(key,value);
                    }
                }
            }
        }
        // Save updated content to sd card
        saveToConfigLines(lines,true);
        filesystems->saveConfigFile(lines);
    }
    // Save preferences after init
    preferences.end();
    preferences.begin(PREF_PREFIX, false);
}

void Settings::setSettingValue(String key, String value)
{
    //Serial.printf("Key=%s; value=%s\n",key.c_str(),value.c_str());
    if(!value.isEmpty())
    {
        if(key.equals(BLUETOOTH_STATE.configKey))
        {
            setBluetoothState(stringToBool(value));
        }
        else if(key.equals(WIFI_STATE.configKey))
        {
            setWifiState(stringToBool(value));
        }
        else if(key.equals(WIFI_SSID.configKey))
        {
            setWifiSsid(value);
        }
        else if(key.equals(WIFI_PASS_PHRASE.configKey))
        {
            setWifiPassPhrase(value);
        }
        else if(key.equals(WIFI_SSID1.configKey))
        {
            setWifiSsid1(value);
        }
        else if(key.equals(WIFI_PASS_PHRASE1.configKey))
        {
            setWifiPassPhrase1(value);
        }
        else if(key.equals(WIFI_SSID2.configKey))
        {
            setWifiSsid2(value);
        }
        else if(key.equals(WIFI_PASS_PHRASE2.configKey))
        {
            setWifiPassPhrase2(value);
        }
        else if(key.equals(TIME_ZONE.configKey))
        {
            setTimeZone(value);
        }
        else if(key.equals(DISPLAY_REVERSE.configKey))
        {
            setDisplayReverse(stringToBool(value));
        }
        else if(key.equals(SCREEN_OFF_ON_CHRAGE.configKey))
        {
            setScreenOffOnCharge(stringToBool(value));
        }
        else if(key.equals(SHOW_BAT_PERCENTAGE.configKey))
        {
            setShowBatteryPercentage(stringToBool(value));
        }
        else if(key.equals(SHOW_BAT_WARN_MESSAGE.configKey))
        {
            setShowBatteryWarnMessage(stringToBool(value));
        }
        else if(key.equals(BUZZER_LEVEL.configKey))
        {
            setBuzzerLevel(stringToUChar(value));
        }
        else if(key.equals(TOUCH_SOUND.configKey))
        {
            setTouchSound(stringToBool(value));
        }
        else if(key.equals(FRAME_SOUND.configKey))
        {
            setFrameSound(stringToBool(value));
        }
        else if(key.equals(COUNTDOWN_SOUND.configKey))
        {
            setCountDownSound(stringToBool(value));
        }
        else if(key.equals(COUNTDOWN_LEDS.configKey))
        {
            setCountDownLeds(stringToBool(value));
        }
        else if(key.equals(RELOAD_COUNTDOWN.configKey))
        {
            setReloadCountDown(stringToBool(value));
        }
        else if(key.equals(COUNTDOWN_DURATION.configKey))
        {
            setCountdownDuration(stringToUChar(value));
        }
        else if(key.equals(ALLOW_FRAME_SIMU.configKey))
        {
            setAllowFrameSimu(stringToBool(value));
        }
        else if(key.equals(FILTER_INVALID.configKey))
        {
            setFilterInvalid(stringToBool(value));
        }
        else if(key.equals(FILTER_ORBITO.configKey))
        {
            setFilterOrbito(stringToBool(value));
        }
    }
}


void Settings::saveToConfigLines(std::vector<String>& lines, bool keepContent)
{
    updateConfigLine(lines,keepContent,BLUETOOTH_STATE      ,boolToString(bluetoothState));
    updateConfigLine(lines,keepContent,WIFI_STATE           ,boolToString(wifiState));
    updateConfigLine(lines,keepContent,WIFI_SSID            ,wifiSsid);
    updateConfigLine(lines,keepContent,WIFI_PASS_PHRASE     ,wifiPassPhrase);
    updateConfigLine(lines,keepContent,WIFI_SSID1           ,wifiSsid1);
    updateConfigLine(lines,keepContent,WIFI_PASS_PHRASE1    ,wifiPassPhrase1);
    updateConfigLine(lines,keepContent,WIFI_SSID2           ,wifiSsid2);
    updateConfigLine(lines,keepContent,WIFI_PASS_PHRASE2    ,wifiPassPhrase2);
    updateConfigLine(lines,keepContent,TIME_ZONE            ,timeZone);
    updateConfigLine(lines,keepContent,DISPLAY_REVERSE      ,boolToString(displayReverse));
    updateConfigLine(lines,keepContent,SCREEN_OFF_ON_CHRAGE ,boolToString(screenOffOnCharge));
    updateConfigLine(lines,keepContent,SHOW_BAT_PERCENTAGE  ,boolToString(showBatteryPercentage));
    updateConfigLine(lines,keepContent,SHOW_BAT_WARN_MESSAGE,boolToString(showBatteryWarnMessage));
    updateConfigLine(lines,keepContent,BUZZER_LEVEL         ,ucharToString(buzzerLevel));
    updateConfigLine(lines,keepContent,TOUCH_SOUND          ,boolToString(touchSound));
    updateConfigLine(lines,keepContent,FRAME_SOUND          ,boolToString(frameSound));
    updateConfigLine(lines,keepContent,COUNTDOWN_SOUND      ,boolToString(countDownSound));
    updateConfigLine(lines,keepContent,COUNTDOWN_LEDS       ,boolToString(countDownLeds));
    updateConfigLine(lines,keepContent,RELOAD_COUNTDOWN     ,boolToString(reloadCountDown));
    updateConfigLine(lines,keepContent,COUNTDOWN_DURATION   ,ucharToString(countdownDuration));
    updateConfigLine(lines,keepContent,ALLOW_FRAME_SIMU     ,boolToString(allowFrameSimu));
    updateConfigLine(lines,keepContent,FILTER_INVALID       ,boolToString(filterInvalid));
    updateConfigLine(lines,keepContent,FILTER_ORBITO        ,boolToString(filterOrbito));
}

void Settings::updateConfigLine(std::vector<String>& lines, bool keepContent,const Setting& setting, const String& value)
{
    String prefix = setting.configKey + "=";
    if(keepContent)
    {
        bool found = false;
        for (size_t i = 0; i < lines.size(); i++) 
        {
            if (lines[i].startsWith(prefix)) 
            {   // Line found
                lines[i] = prefix + value; // replace existing value
                found = true;
                break;
            }
        }
        if (!found) 
        {   // append new key and comment
            lines.push_back("# "+setting.desciption);
            lines.push_back(prefix + value);
        }
    }
    else
    {
        lines.push_back(prefix + value);
    }
}

void Settings::saveToSd()
{
    Filesystems* filesystems = Filesystems::getFilesystems();
    if(filesystems->isSdFilesystemMounted())
    {   // Read current config file content
        std::vector<String> lines;
        filesystems->loadConfigFile(lines);
        saveToConfigLines(lines,true);
        filesystems->saveConfigFile(lines);
    }
}

String Settings::toKvpString()
{
    String result;
    std::vector<String> lines;
    saveToConfigLines(lines,false);
    for (auto line : lines) 
    {   // Save lines
        result += line;
        result += "\n";
    }
    return result;
}


bool Settings::getBluetoothState()
{
    return bluetoothState;
}

void Settings::setBluetoothState(bool state)
{
    if(bluetoothState == state) return;
    bluetoothState = state;
    preferences.putBool(BLUETOOTH_STATE.key.c_str(),state);
    dirty = true;
}

bool Settings::getWifiState()
{
    return wifiState;
}

void Settings::setWifiState(bool state)
{
    if(wifiState == state) return;
    wifiState = state;
    preferences.putBool(WIFI_STATE.key.c_str(),state);
    dirty = true;
}

String Settings::getWifiSsid()
{
    return wifiSsid;
}

void Settings::setWifiSsid(String ssid)
{
    if(wifiSsid.equals(ssid)) return;
    wifiSsid = ssid;
    preferences.putString(WIFI_SSID.key.c_str(),ssid);
    dirty = true;
}

String Settings::getWifiPassPhrase()
{
    return wifiPassPhrase;
}

void Settings::setWifiPassPhrase(String passPhrase)
{
    if(wifiPassPhrase.equals(passPhrase)) return;
    wifiPassPhrase = passPhrase;
    preferences.putString(WIFI_PASS_PHRASE.key.c_str(),passPhrase);
    dirty = true;
}

String Settings::getWifiSsid1()
{
    return wifiSsid1;
}

void Settings::setWifiSsid1(String ssid)
{
    if(wifiSsid1.equals(ssid)) return;
    wifiSsid1 = ssid;
    preferences.putString(WIFI_SSID1.key.c_str(),ssid);
    dirty = true;
}

String Settings::getWifiPassPhrase1()
{
    return wifiPassPhrase1;
}

void Settings::setWifiPassPhrase1(String passPhrase)
{
    if(wifiPassPhrase1.equals(passPhrase)) return;
    wifiPassPhrase1 = passPhrase;
    preferences.putString(WIFI_PASS_PHRASE1.key.c_str(),passPhrase);
    dirty = true;
}

String Settings::getWifiSsid2()
{
    return wifiSsid2;
}

void Settings::setWifiSsid2(String ssid)
{
    if(wifiSsid2.equals(ssid)) return;
    wifiSsid2 = ssid;
    preferences.putString(WIFI_SSID2.key.c_str(),ssid);
    dirty = true;
}

String Settings::getWifiPassPhrase2()
{
    return wifiPassPhrase2;
}

void Settings::setWifiPassPhrase2(String passPhrase)
{
    if(wifiPassPhrase2.equals(passPhrase)) return;
    wifiPassPhrase2 = passPhrase;
    preferences.putString(WIFI_PASS_PHRASE2.key.c_str(),passPhrase);
    dirty = true;
}

String Settings::getTimeZone()
{
    return timeZone;
}

void Settings::setTimeZone(String tz)
{
    if(timeZone.equals(tz)) return;
    timeZone = tz;
    preferences.putString(TIME_ZONE.key.c_str(),tz);
    dirty = true;
}

bool Settings::getDisplayReverse()
{
    return displayReverse;
}

void Settings::setDisplayReverse(bool state)
{
    if(displayReverse == state) return;
    displayReverse = state;
    preferences.putBool(DISPLAY_REVERSE.key.c_str(),state);
    dirty = true;
}

bool Settings::getScreenOffOnCharge()
{
    return screenOffOnCharge;
}

void Settings::setScreenOffOnCharge(bool active)
{
    if(screenOffOnCharge == active) return;
    screenOffOnCharge = active;
    preferences.putBool(SCREEN_OFF_ON_CHRAGE.key.c_str(),active);
    dirty = true;
}

bool Settings::getShowBatteryPercentage()
{
    return showBatteryPercentage;
}

void Settings::setShowBatteryPercentage(bool show)
{
    if(showBatteryPercentage == show) return;
    showBatteryPercentage = show;
    preferences.putBool(SHOW_BAT_PERCENTAGE.key.c_str(),show);
    dirty = true;
}

bool Settings::getShowBatteryWarnMessage()
{
    return showBatteryWarnMessage;
}

void Settings::setShowBatteryWarnMessage(bool show)
{
    if(showBatteryWarnMessage == show) return;
    showBatteryWarnMessage = show;
    preferences.putBool(SHOW_BAT_WARN_MESSAGE.key.c_str(),show);
    dirty = true;
}

uint8_t Settings::getBuzzerLevel()
{
    return buzzerLevel;
}

void Settings::setBuzzerLevel(uint8_t level)
{
    if(buzzerLevel == level) return;
    buzzerLevel = level;
    preferences.putUChar(BUZZER_LEVEL.key.c_str(),level);
    dirty = true;
}

bool Settings::getTouchSound()
{
    return touchSound;
}

void Settings::setTouchSound(bool active)
{
    if(touchSound == active) return;
    touchSound = active;
    preferences.putBool(TOUCH_SOUND.key.c_str(),active);
    dirty = true;
}

bool Settings::getFrameSound()
{
    return frameSound;
}

void Settings::setFrameSound(bool active)
{
    if(frameSound == active) return;
    frameSound = active;
    preferences.putBool(FRAME_SOUND.key.c_str(),active);
    dirty = true;
}

bool Settings::getCountDownSound()
{
    return countDownSound;
}

void Settings::setCountDownSound(bool active)
{
    if(countDownSound == active) return;
    countDownSound = active;
    preferences.putBool(COUNTDOWN_SOUND.key.c_str(),active);
    dirty = true;
}

bool Settings::getCountDownLeds()
{
    return countDownLeds;
}

void Settings::setCountDownLeds(bool active)
{
    if(countDownLeds == active) return;
    countDownLeds = active;
    preferences.putBool(COUNTDOWN_LEDS.key.c_str(),active);
    dirty = true;
}

bool Settings::getReloadCountDown()
{
    return reloadCountDown;
}

void Settings::setReloadCountDown(bool active)
{
    if(reloadCountDown == active) return;
    reloadCountDown = active;
    preferences.putBool(RELOAD_COUNTDOWN.key.c_str(),active);
    dirty = true;
}

uint8_t Settings::getCountdownDuration()
{
    return countdownDuration;
}

void Settings::setCountdownDuration(uint8_t duration)
{
    if(countdownDuration == duration) return;
    countdownDuration = duration;
    preferences.putUChar(COUNTDOWN_DURATION.key.c_str(),duration);
    dirty = true;
}

bool Settings::getAllowFrameSimu()
{
    return allowFrameSimu;
}

void Settings::setAllowFrameSimu(bool active)
{
    if(allowFrameSimu == active) return;
    allowFrameSimu = active;
    preferences.putBool(ALLOW_FRAME_SIMU.key.c_str(),active);
    dirty = true;
}

bool Settings::getFilterOrbito()
{
    return filterOrbito;
}

void Settings::setFilterOrbito(bool active)
{
    if(filterOrbito == active) return;
    filterOrbito = active;
    preferences.putBool(FILTER_ORBITO.key.c_str(),active);
    dirty = true;
}

bool Settings::getFilterInvalid()
{
    return filterInvalid;
}

void Settings::setFilterInvalid(bool active)
{
    if(filterInvalid == active) return;
    filterInvalid = active;
    preferences.putBool(FILTER_INVALID.key.c_str(),active);
    dirty = true;
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
        saveToSd();
        dirty = false;
    }
}

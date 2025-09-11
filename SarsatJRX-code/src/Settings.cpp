#include <Settings.h>
#include <SarsatJRXConf.h>
#include <Filesystems.h>
#include <Util.h>

#define PREF_PREFIX             "SarsatJRX"

Settings *Settings::settingsInstance = nullptr;

static const Settings::Setting WIFI_STATE           ("Wifi" ,"wifiState"            ,"Wifi on (true/false) ?");
static const Settings::Setting WIFI_SSID            ("SSID" ,"wifiSsid"             ,"Ssid of the wifi network to connect to");
static const Settings::Setting WIFI_PASS_PHRASE     ("PSK"  ,"wifiPassPhrase"       ,"Passphrase of the Wifi network to connect to");
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
    wifiState = preferences.getBool(WIFI_STATE.key.c_str(),false);
    wifiSsid = preferences.getString(WIFI_SSID.key.c_str(),"SarsatJRX");
    wifiPassPhrase = preferences.getString(WIFI_PASS_PHRASE.key.c_str(),"SarsatJRX");
    wifiPassPhrase = preferences.getBool(WIFI_STATE.key.c_str(),false);
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
                        //Serial.printf("Key=%s; value=%s\n",key.c_str(),value.c_str());
                        if(!value.isEmpty())
                        {
                            if(key.equals(WIFI_STATE.configKey))
                            {
                                wifiState = stringToBool(value);
                            }
                            else if(key.equals(WIFI_SSID.configKey))
                            {
                                wifiSsid = value;
                            }
                            else if(key.equals(WIFI_PASS_PHRASE.configKey))
                            {
                                wifiPassPhrase = value;
                            }
                            else if(key.equals(DISPLAY_REVERSE.configKey))
                            {
                                displayReverse = stringToBool(value);
                            }
                            else if(key.equals(SCREEN_OFF_ON_CHRAGE.configKey))
                            {
                                screenOffOnCharge = stringToBool(value);
                            }
                            else if(key.equals(SHOW_BAT_PERCENTAGE.configKey))
                            {
                                showBatteryPercentage = stringToBool(value);
                            }
                            else if(key.equals(SHOW_BAT_WARN_MESSAGE.configKey))
                            {
                                showBatteryWarnMessage = stringToBool(value);
                            }
                            else if(key.equals(BUZZER_LEVEL.configKey))
                            {
                                buzzerLevel = stringToUChar(value);
                            }
                            else if(key.equals(TOUCH_SOUND.configKey))
                            {
                                touchSound = stringToBool(value);
                            }
                            else if(key.equals(FRAME_SOUND.configKey))
                            {
                                frameSound = stringToBool(value);
                            }
                            else if(key.equals(COUNTDOWN_SOUND.configKey))
                            {
                                countDownSound = stringToBool(value);
                            }
                            else if(key.equals(COUNTDOWN_LEDS.configKey))
                            {
                                countDownLeds = stringToBool(value);
                            }
                            else if(key.equals(RELOAD_COUNTDOWN.configKey))
                            {
                                reloadCountDown = stringToBool(value);
                            }
                            else if(key.equals(COUNTDOWN_DURATION.configKey))
                            {
                                countdownDuration = stringToUChar(value);
                            }
                            else if(key.equals(ALLOW_FRAME_SIMU.configKey))
                            {
                                allowFrameSimu = stringToBool(value);
                            }
                            else if(key.equals(FILTER_INVALID.configKey))
                            {
                                filterInvalid = stringToBool(value);
                            }
                            else if(key.equals(FILTER_ORBITO.configKey))
                            {
                                filterOrbito = stringToBool(value);
                            }
                        }
                    }
                }
            }
        }
        // Save updated content
        saveToConfigLines(lines);
        filesystems->saveConfigFile(lines);
    }

}

void Settings::saveToConfigLines(std::vector<String>& lines)
{
    updateConfigLine(lines,WIFI_STATE           ,boolToString(wifiState));
    updateConfigLine(lines,WIFI_SSID            ,wifiSsid);
    updateConfigLine(lines,WIFI_PASS_PHRASE     ,wifiPassPhrase);
    updateConfigLine(lines,DISPLAY_REVERSE      ,boolToString(displayReverse));
    updateConfigLine(lines,SCREEN_OFF_ON_CHRAGE ,boolToString(screenOffOnCharge));
    updateConfigLine(lines,SHOW_BAT_PERCENTAGE  ,boolToString(showBatteryPercentage));
    updateConfigLine(lines,SHOW_BAT_WARN_MESSAGE,boolToString(showBatteryWarnMessage));
    updateConfigLine(lines,BUZZER_LEVEL         ,ucharToString(buzzerLevel));
    updateConfigLine(lines,TOUCH_SOUND          ,boolToString(touchSound));
    updateConfigLine(lines,FRAME_SOUND          ,boolToString(frameSound));
    updateConfigLine(lines,COUNTDOWN_SOUND      ,boolToString(countDownSound));
    updateConfigLine(lines,COUNTDOWN_LEDS       ,boolToString(countDownLeds));
    updateConfigLine(lines,RELOAD_COUNTDOWN     ,boolToString(reloadCountDown));
    updateConfigLine(lines,COUNTDOWN_DURATION   ,ucharToString(countdownDuration));
    updateConfigLine(lines,ALLOW_FRAME_SIMU     ,boolToString(allowFrameSimu));
    updateConfigLine(lines,FILTER_INVALID       ,boolToString(filterInvalid));
    updateConfigLine(lines,FILTER_ORBITO        ,boolToString(filterOrbito));
}

void Settings::updateConfigLine(std::vector<String>& lines,const Setting& setting, const String& value)
{
    String prefix = setting.configKey + "=";
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

void Settings::saveToSd()
{
    Filesystems* filesystems = Filesystems::getFilesystems();
    if(filesystems->isSdFilesystemMounted())
    {   // Read current config file content
        std::vector<String> lines;
        filesystems->loadConfigFile(lines);
        saveToConfigLines(lines);
        filesystems->saveConfigFile(lines);
    }
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

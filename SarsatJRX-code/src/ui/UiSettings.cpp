#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>
#include <ui/Ui.h>
#include <ui/UiBeacon.h>
#include <Hardware.h>
#include <WiFi.h>
#include <WifiManager.h>
#include <Settings.h>

// System info
#define SPACER              8
#define HALF_SPACER         4
#define VERSION_LABEL       "Version :"
#define VERSION_LABEL_WIDTH 80
#define SKETCH_LABEL        "Sketch :"
#define SKETCH_LABEL_WIDTH  80
#define VBAT_LABEL          "Vbat :"
#define VBAT_LABEL_WIDTH    80

#define CHIP_LABEL          "Chip :"
#define CHIP_LABEL_WIDTH    50
#define CHIPM_LABEL         "- Model:"
#define CHIPM_LABEL_WIDTH   70
#define CHIPR_LABEL         "- Rev.:"
#define CHIPR_LABEL_WIDTH   70
#define CHIPC_LABEL         "- Cores:"
#define CHIPC_LABEL_WIDTH   70
#define CHIPF_LABEL         "- Freq.:"
#define CHIPF_LABEL_WIDTH   70

#define RAM_LABEL          "Ram :"
#define RAM_LABEL_WIDTH    50
#define RAMS_LABEL         "- Total:"
#define RAMS_LABEL_WIDTH   70
#define RAMF_LABEL         "- Free:"
#define RAMF_LABEL_WIDTH   70

#define PSRAM_LABEL          "PS-Ram :"
#define PSRAM_LABEL_WIDTH    75
#define PSRAMS_LABEL         "- Total:"
#define PSRAMS_LABEL_WIDTH   75
#define PSRAMF_LABEL         "- Free:"
#define PSRAMF_LABEL_WIDTH   75

#define FLASH_LABEL          "Flash :"
#define FLASH_LABEL_WIDTH    75
#define FLASHS_LABEL         "- Size:"
#define FLASHS_LABEL_WIDTH   75
#define FLASHF_LABEL         "- Speed:"
#define FLASHF_LABEL_WIDTH   75

// Wifi / Net
#define TOGGLE_LINE_HEIGHT  28
#define TOGGLE_X            160
#define TOGGLE_WIDTH        50
#define WIFI_LABEL          "Wifi :"
#define WIFI_LABEL_WIDTH    80
#define PORTAL_LABEL        "Wifi setup portal :"
#define PORTAL_LABEL_WIDTH  200
#define MODE_LABEL          "Mode :"
#define MODE_LABEL_WIDTH    80
#define STATUS_LABEL        "Status :"
#define STATUS_LABEL_WIDTH  80
#define SIGNAL_LABEL        "Signal :"
#define SIGNAL_LABEL_WIDTH  80
#define SSID_LABEL          "SSID :"
#define SSID_LABEL_WIDTH    80
#define MAC_LABEL           "MAC :"
#define MAC_LABEL_WIDTH     80
#define IP_LABEL            "IP :"
#define IP_LABEL_WIDTH      80
#define GATEWAY_LABEL       "Gateway :"
#define GATEWAY_LABEL_WIDTH 80
#define DNS_LABEL           "DNS :"
#define DNS1_LABEL          "DNS 1 :"
#define DNS1_LABEL_WIDTH    80
#define DNS2_LABEL          "DNS 2 :"
#define DNS2_LABEL_WIDTH    80
#define MASK_LABEL          "Mask :"
#define MASK_LABEL_WIDTH    80
// NTP status
#define NTP_LABEL           "NTP :"
#define NTP_LABEL_WIDTH     50
#define NTP_DATE_LABEL      "- Date:"
#define NTP_DATE_WIDTH      70
#define NTP_SRV_LABEL       "- Server:"
#define NTP_SRV_WIDTH       70
#define NTP_SYNC_LABEL      "- Sync.:"
#define NTP_SYNC_WIDTH      70

// SD card
#define SD_LABEL                "SD card mounted :"
#define SD_LABEL_WIDTH          160
#define TOTAL_SIZE_LABEL        "Total size :"
#define TOTAL_SIZE_LABEL_WIDTH  100
#define USED_SIZE_LABEL         "Used size :"
#define USED_SIZE_LABEL_WIDTH   100
#define LOG_FOLDER_LABEL        "Beacons folder :"
#define LOG_FOLDER_LABEL_WIDTH  160
#define BEACONS_LABEL           "Beacons :"
#define BEACONS_LABEL_WIDTH     100
#define BEACON_LIST_WIDTH       260
#define BEACON_BUTTON_WIDTH     70
#define BEACON_BUTTON_HEIGHT    50
#define BEACON_BUTTON_X1        (BEACON_LIST_WIDTH + 4)
#define BEACON_BUTTON_X2        (BEACON_BUTTON_X1 + BEACON_BUTTON_WIDTH + 4)

// Radio
#define RADIO_TOGGLE_X          0
#define RADIO_BUTTONS_WIDTH     40
#define RADIO_FREQ_NEXT_X       (TOGGLE_WIDTH+4)
#define RADIO_FREQ_X            (RADIO_FREQ_NEXT_X+RADIO_BUTTONS_WIDTH+4)
#define RADIO_FREQ_HEIGHT       (TOGGLE_LINE_HEIGHT+2*SPACER)
#define RADIO_METER_HEIGHT      10
#define FILTERS_LABEL           "Filters :"
#define FILTERS_LABEL_WIDTH     60
#define FILTER1_LABEL           "Emph."
#define FILTER2_LABEL           "HiP."
#define FILTER3_LABEL           "LoP."
#define FILTER_LABEL_WIDTH      50


// Externs
extern bool readBeaconFromFile(const char * filename);

lv_obj_t * settingsTabview;

// System
static lv_obj_t * versionTitle;
static lv_obj_t * versionLabel;
static lv_obj_t * sketchTitle;
static lv_obj_t * sketchLabel;
static lv_obj_t * vBatTitle;
static lv_obj_t * vBatLabel;
static lv_obj_t * chipTitle;
static lv_obj_t * chipModelTitle;
static lv_obj_t * chipModelLabel;
static lv_obj_t * chipRevisionTitle;
static lv_obj_t * chipRevisionLabel;
static lv_obj_t * chipCoresTitle;
static lv_obj_t * chipCoresLabel;
static lv_obj_t * chipFreqTitle;
static lv_obj_t * chipFreqLabel;
static lv_obj_t * ramTitle;
static lv_obj_t * ramSizeTitle;
static lv_obj_t * ramSizeLabel;
static lv_obj_t * ramFreeTitle;
static lv_obj_t * ramFreeLabel;
static lv_obj_t * psRamTitle;
static lv_obj_t * psRamSizeTitle;
static lv_obj_t * psRamSizeLabel;
static lv_obj_t * psRamFreeTitle;
static lv_obj_t * psRamFreeLabel;
static lv_obj_t * flashTitle;
static lv_obj_t * flashSizeTitle;
static lv_obj_t * flashSizeLabel;
static lv_obj_t * flashFreqTitle;
static lv_obj_t * flashFreqLabel;

// Wifi
static lv_obj_t * wifiTitle;
static lv_obj_t * wifiToggle;
static lv_obj_t * portalTitle;
static lv_obj_t * portalToggle;
static lv_obj_t * modeTitle;
static lv_obj_t * modeLabel;
static lv_obj_t * statusTitle;
static lv_obj_t * statusLabel;
static lv_obj_t * signalTitle;
static lv_obj_t * signalLabel;
static lv_obj_t * ssidTitle;
static lv_obj_t * ssidLabel;
static lv_obj_t * ipTitle;
static lv_obj_t * ipLabel;
static lv_obj_t * dnsTitle;
static lv_obj_t * dnsLabel;
static lv_obj_t * gatewayTitle;
static lv_obj_t * gatewayLabel;

// Network
static lv_obj_t * netStatusTitle;
static lv_obj_t * netStatusLabel;
static lv_obj_t * macTitle;
static lv_obj_t * macLabel;
static lv_obj_t * netIpTitle;
static lv_obj_t * netIpLabel;
static lv_obj_t * netGatewayTitle;
static lv_obj_t * netGatewayLabel;
static lv_obj_t * dns1Title;
static lv_obj_t * dns1Label;
static lv_obj_t * dns2Title;
static lv_obj_t * dns2Label;
static lv_obj_t * maskTitle;
static lv_obj_t * maskLabel;
static lv_obj_t * ntpTitle;
static lv_obj_t * ntpDateTitle;
static lv_obj_t * ntpDateLabel;
static lv_obj_t * ntpServerTitle;
static lv_obj_t * ntpServerLabel;
static lv_obj_t * ntpSyncTitle;
static lv_obj_t * ntpSyncLabel;

// SD
static lv_obj_t * sdTitle;
static lv_obj_t * sdToggle;
static lv_obj_t * totalBytesTitle;
static lv_obj_t * totalBytesLabel;
static lv_obj_t * usedBytesTitle;
static lv_obj_t * usedBytesLabel;
static lv_obj_t * logFolderTitle;
static lv_obj_t * logFolderLabel;
static lv_obj_t * beaconsTitle;
static lv_obj_t * beaconsList;
static lv_obj_t * beaconsListButtons;
static lv_obj_t * beaconsUpButton;
static lv_obj_t * beaconsDownButton;
static lv_obj_t * beaconsLoadButton;
static lv_obj_t * beaconsDeleteButton;
static lv_obj_t * currentBeacon = NULL;

static lv_obj_t * deleteConfirmBox;
static int lastBeaconIndex = 0;

// Radio
static lv_obj_t * radioTab;
static int radioTabWidth;
static int radioTabHeight;
static lv_obj_t * radioToggle;
static lv_obj_t * radiioFreqLabelButton;
static lv_obj_t * freqPrevButton;
static lv_obj_t * freqNextButton;
static lv_obj_t * radioFreqTextArea;
static lv_obj_t * radioFreqKeyboard;
static lv_obj_t * radioMeter;
static lv_obj_t * radioVersionTitle;
static lv_obj_t * radioVersionLabel;
static lv_obj_t * radioFiltersTitle;
static lv_obj_t * radioFilter1Toggle;
static lv_obj_t * radioFilter1Label;
static lv_obj_t * radioFilter2Toggle;
static lv_obj_t * radioFilter2Label;
static lv_obj_t * radioFilter3Toggle;
static lv_obj_t * radioFilter3Label;

void createSystemTab(lv_obj_t * tab, int currentY, int tabWidth)
{  
    // Version
    versionTitle = uiCreateLabel(tab,&style_section_title,VERSION_LABEL,0,currentY,VERSION_LABEL_WIDTH,LINE_HEIGHT);
    versionLabel = uiCreateLabel(tab,&style_section_text,SARSAT_JRX_VERSION,VERSION_LABEL_WIDTH,currentY,tabWidth-VERSION_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Sketch
    sketchTitle = uiCreateLabel(tab,&style_section_title,SKETCH_LABEL,0,currentY,SKETCH_LABEL_WIDTH,LINE_HEIGHT);
    sketchLabel = uiCreateLabel(tab,&style_section_text,formatSketchInformation(ESP.getSketchSize(),ESP.getSketchMD5()).c_str(),SKETCH_LABEL_WIDTH,currentY,tabWidth-SKETCH_LABEL_WIDTH,LINE_HEIGHT);
    // lv_label_set_long_mode(sketchLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    currentY+=LINE_HEIGHT;

    // Vbat
    vBatTitle = uiCreateLabel(tab,&style_section_title,VBAT_LABEL,0,currentY,VBAT_LABEL_WIDTH,LINE_HEIGHT);
    vBatLabel = uiCreateLabel(tab,&style_section_text,"",VBAT_LABEL_WIDTH,currentY,tabWidth-VBAT_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Chip
    chipTitle = uiCreateLabel(tab,&style_section_title,CHIP_LABEL,0,currentY,CHIP_LABEL_WIDTH,LINE_HEIGHT);
    chipModelTitle = uiCreateLabel(tab,&style_section_title,CHIPM_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPM_LABEL_WIDTH,LINE_HEIGHT);
    chipModelLabel = uiCreateLabel(tab,&style_section_text,ESP.getChipModel(),CHIP_LABEL_WIDTH+CHIPM_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPM_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipRevisionTitle = uiCreateLabel(tab,&style_section_title,CHIPR_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPR_LABEL_WIDTH,LINE_HEIGHT);
    chipRevisionLabel = uiCreateLabel(tab,&style_section_text,(String(ESP.getChipRevision())).c_str(),CHIP_LABEL_WIDTH+CHIPR_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPR_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipCoresTitle = uiCreateLabel(tab,&style_section_title,CHIPC_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPC_LABEL_WIDTH,LINE_HEIGHT);
    chipCoresLabel = uiCreateLabel(tab,&style_section_text,(String(ESP.getChipCores())).c_str(),CHIP_LABEL_WIDTH+CHIPC_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPC_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipFreqTitle = uiCreateLabel(tab,&style_section_title,CHIPF_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPF_LABEL_WIDTH,LINE_HEIGHT);
    chipFreqLabel = uiCreateLabel(tab,&style_section_text,formatFrequencyValue(ESP.getCpuFreqMHz()).c_str(),CHIP_LABEL_WIDTH+CHIPF_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Ram
    ramTitle = uiCreateLabel(tab,&style_section_title,RAM_LABEL,0,currentY,RAM_LABEL_WIDTH,LINE_HEIGHT);
    ramSizeTitle = uiCreateLabel(tab,&style_section_title,RAMS_LABEL,RAM_LABEL_WIDTH,currentY,RAMS_LABEL_WIDTH,LINE_HEIGHT);
    ramSizeLabel = uiCreateLabel(tab,&style_section_text,ESP.getChipModel(),RAM_LABEL_WIDTH+RAMS_LABEL_WIDTH,currentY,tabWidth-RAM_LABEL_WIDTH-RAMS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    ramFreeTitle = uiCreateLabel(tab,&style_section_title,RAMF_LABEL,RAM_LABEL_WIDTH,currentY,RAMF_LABEL_WIDTH,LINE_HEIGHT);
    ramFreeLabel = uiCreateLabel(tab,&style_section_text,"",RAM_LABEL_WIDTH+RAMF_LABEL_WIDTH,currentY,tabWidth-RAM_LABEL_WIDTH-RAMF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // PS-Ram
    psRamTitle = uiCreateLabel(tab,&style_section_title,PSRAM_LABEL,0,currentY,PSRAM_LABEL_WIDTH,LINE_HEIGHT);
    psRamSizeTitle = uiCreateLabel(tab,&style_section_title,PSRAMS_LABEL,PSRAM_LABEL_WIDTH,currentY,PSRAMS_LABEL_WIDTH,LINE_HEIGHT);
    psRamSizeLabel = uiCreateLabel(tab,&style_section_text,"",PSRAM_LABEL_WIDTH+PSRAMS_LABEL_WIDTH,currentY,tabWidth-PSRAM_LABEL_WIDTH-PSRAMS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    psRamFreeTitle = uiCreateLabel(tab,&style_section_title,PSRAMF_LABEL,PSRAM_LABEL_WIDTH,currentY,PSRAMF_LABEL_WIDTH,LINE_HEIGHT);
    psRamFreeLabel = uiCreateLabel(tab,&style_section_text,"",PSRAM_LABEL_WIDTH+PSRAMF_LABEL_WIDTH,currentY,tabWidth-PSRAM_LABEL_WIDTH-PSRAMF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Flash
    flashTitle = uiCreateLabel(tab,&style_section_title,FLASH_LABEL,0,currentY,FLASH_LABEL_WIDTH,LINE_HEIGHT);
    flashSizeTitle = uiCreateLabel(tab,&style_section_title,FLASHS_LABEL,FLASH_LABEL_WIDTH,currentY,FLASHS_LABEL_WIDTH,LINE_HEIGHT);
    flashSizeLabel = uiCreateLabel(tab,&style_section_text,"",FLASH_LABEL_WIDTH+FLASHS_LABEL_WIDTH,currentY,tabWidth-FLASH_LABEL_WIDTH-FLASHS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    flashFreqTitle = uiCreateLabel(tab,&style_section_title,FLASHF_LABEL,FLASH_LABEL_WIDTH,currentY,FLASHF_LABEL_WIDTH,LINE_HEIGHT);
    flashFreqLabel = uiCreateLabel(tab,&style_section_text,"",FLASH_LABEL_WIDTH+FLASHF_LABEL_WIDTH,currentY,tabWidth-FLASH_LABEL_WIDTH-FLASHF_LABEL_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;
}

static void toggle_wifi_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(wifiToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle wifi :" + String(state));
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setWifiState(state);
    if(state)
    {
        wifiManagerStart();
    }
    else
    {
        wifiManagerStop();
    }
}

static void toggle_portal_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(portalToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle portal :" + String(state));
    if(state)
    {
        wifiManagerStartPortal();
    }
    else
    {
        wifiManagerStart();
    }
}

static void toggle_sd_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(sdToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle sd :" + String(state));
    Filesystems* filesystems = Filesystems::getFilesystems();
    if(state)
    {
        filesystems->initSdFs();
        uiSetSdCardStatus(filesystems->isSdFilesystemMounted());
        uiSettingsUpdateView();
    }
    else
    {
        filesystems->unmountSdFs();
        uiSetSdCardStatus(filesystems->isSdFilesystemMounted());
        uiSettingsUpdateView();
    }
}

static int beaconCount = 0;
void uiUpdateLogFolder()
{   // Log folder
    Filesystems *filesystems =  Hardware::getHardware()->getFilesystems();
    lv_label_set_text(logFolderLabel,(filesystems->isLogDirReady() ? ("OK (" + String(beaconCount) + " beacon(s))").c_str() : "KO"));
    lv_obj_set_style_text_color(logFolderLabel, (filesystems->isLogDirReady() ? uiOkColor : uiKoColor),0);
}


static void selectBeacon(lv_obj_t* beacon)
{
    if(beacon)
    {
        lv_obj_add_state(beacon, LV_STATE_CHECKED);
        if((currentBeacon != NULL) && (currentBeacon != beacon))
        {
            lv_obj_clear_state(currentBeacon, LV_STATE_CHECKED);
        }
        currentBeacon = beacon;
        lastBeaconIndex = lv_obj_get_index(currentBeacon);
    }
}

static void beacon_clicked_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    selectBeacon(obj);
}

static void beacon_up_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        uint32_t index = lv_obj_get_index(currentBeacon);
        if(index <= 0) return;
        index--;
        selectBeacon(lv_obj_get_child(beaconsList,index));
        lv_obj_scroll_to_view(currentBeacon, LV_ANIM_ON);
    }    
}

static void beacon_down_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        uint32_t index = lv_obj_get_index(currentBeacon);
        index++;
        lv_obj_t *obj = lv_obj_get_child(beaconsList,index);
        if(obj)
        {
            selectBeacon(obj);
            lv_obj_scroll_to_view(currentBeacon, LV_ANIM_ON);
        }
    }    
}

static void beacon_load_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        String* fileName = (String*)lv_obj_get_user_data(currentBeacon);
        if(fileName)
        {
            //Serial.printf("Load file %s\n",(*fileName).c_str());
            readBeaconFromFile((*fileName).c_str());
        }
    }
}

static void deleteCurrentBeacon()
{
    if(currentBeacon == NULL) return;
    String* fileName = (String*)lv_obj_get_user_data(currentBeacon);
    if(Filesystems::getFilesystems()->deleteBeacon((*fileName).c_str()))
    {
        //Serial.printf("Deleted file %s\n",(*fileName).c_str());
        uint32_t index = lv_obj_get_index(currentBeacon);
        lv_obj_del(currentBeacon);
        currentBeacon = lv_obj_get_child(beaconsList,index);
        if(!currentBeacon)
        {
            currentBeacon = lv_obj_get_child(beaconsList,index-1);
        }
        selectBeacon(currentBeacon);
        // Update beacon count
        beaconCount--;
        uiUpdateLogFolder();
    }
}

static void beacon_delete_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    //lv_event_code_t code = lv_event_get_code(e);
    //Serial.printf("Delete cb with event code %d\n",code);
    //if((code != LV_EVENT_SHORT_CLICKED)) return;
    static const char * btns[] = {"OK", "Cancel", NULL};
    deleteConfirmBox = lv_msgbox_create(NULL, "Delete ?", "Do you want to delete this beacon ?", btns, true);
    lv_obj_add_event_cb(deleteConfirmBox, [](lv_event_t * e) 
    {
        if(lv_msgbox_get_active_btn(deleteConfirmBox) == 0)
        {
            deleteCurrentBeacon(); 
        }
        lv_msgbox_close(deleteConfirmBox);
    }, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(deleteConfirmBox);
}

static void beacon_delete_longpress_cb(lv_event_t * e)
{
    deleteCurrentBeacon();
    lv_event_stop_processing(e);
}

void createWifiTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Wifi On/Off -> save to EEProm
    currentY+=SPACER;
    wifiTitle = uiCreateLabel(tab,&style_section_title,WIFI_LABEL,0,currentY,WIFI_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    wifiToggle = uiCreateToggle(tab,&style_section_text,toggle_wifi_cb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Portal On/Off
    portalTitle = uiCreateLabel(tab,&style_section_title,PORTAL_LABEL,0,currentY,PORTAL_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    portalToggle = uiCreateToggle(tab,&style_section_text,toggle_portal_cb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+SPACER;
    // Mode (Station / Acess Point)
    modeTitle = uiCreateLabel(tab,&style_section_title,MODE_LABEL,0,currentY,MODE_LABEL_WIDTH,LINE_HEIGHT);
    modeLabel = uiCreateLabel(tab,&style_section_text,"",MODE_LABEL_WIDTH,currentY,tabWidth-MODE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Status (Connected etc)
    statusTitle = uiCreateLabel(tab,&style_section_title,STATUS_LABEL,0,currentY,STATUS_LABEL_WIDTH,LINE_HEIGHT);
    statusLabel = uiCreateLabel(tab,&style_section_text,"",STATUS_LABEL_WIDTH,currentY,tabWidth-STATUS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Signal (RSSI)
    signalTitle = uiCreateLabel(tab,&style_section_title,SIGNAL_LABEL,0,currentY,SIGNAL_LABEL_WIDTH,LINE_HEIGHT);
    signalLabel = uiCreateLabel(tab,&style_section_text,"",SIGNAL_LABEL_WIDTH,currentY,tabWidth-SIGNAL_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // SSID
    ssidTitle = uiCreateLabel(tab,&style_section_title,SSID_LABEL,0,currentY,SSID_LABEL_WIDTH,LINE_HEIGHT);
    ssidLabel = uiCreateLabel(tab,&style_section_text,"",SSID_LABEL_WIDTH,currentY,tabWidth-SSID_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // IP @
    ipTitle = uiCreateLabel(tab,&style_section_title,IP_LABEL,0,currentY,IP_LABEL_WIDTH,LINE_HEIGHT);
    ipLabel = uiCreateLabel(tab,&style_section_text,"",IP_LABEL_WIDTH,currentY,tabWidth-IP_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Gateway IP
    gatewayTitle = uiCreateLabel(tab,&style_section_title,GATEWAY_LABEL,0,currentY,GATEWAY_LABEL_WIDTH,LINE_HEIGHT);
    gatewayLabel = uiCreateLabel(tab,&style_section_text,"",GATEWAY_LABEL_WIDTH,currentY,tabWidth-GATEWAY_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // DNS
    dnsTitle = uiCreateLabel(tab,&style_section_title,DNS_LABEL,0,currentY,DNS1_LABEL_WIDTH,LINE_HEIGHT);
    dnsLabel = uiCreateLabel(tab,&style_section_text,"",DNS1_LABEL_WIDTH,currentY,tabWidth-DNS1_LABEL_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;
}

void createNetworkTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Status (Connected etc)
    netStatusTitle = uiCreateLabel(tab,&style_section_title,STATUS_LABEL,0,currentY,STATUS_LABEL_WIDTH,LINE_HEIGHT);
    netStatusLabel = uiCreateLabel(tab,&style_section_text,"",STATUS_LABEL_WIDTH,currentY,tabWidth-STATUS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Mac @
    macTitle = uiCreateLabel(tab,&style_section_title,MAC_LABEL,0,currentY,MAC_LABEL_WIDTH,LINE_HEIGHT);
    macLabel = uiCreateLabel(tab,&style_section_text,"",MAC_LABEL_WIDTH,currentY,tabWidth-MAC_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // IP @
    netIpTitle = uiCreateLabel(tab,&style_section_title,IP_LABEL,0,currentY,IP_LABEL_WIDTH,LINE_HEIGHT);
    netIpLabel = uiCreateLabel(tab,&style_section_text,"",IP_LABEL_WIDTH,currentY,tabWidth-IP_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Gateway IP
    netGatewayTitle = uiCreateLabel(tab,&style_section_title,GATEWAY_LABEL,0,currentY,GATEWAY_LABEL_WIDTH,LINE_HEIGHT);
    netGatewayLabel = uiCreateLabel(tab,&style_section_text,"",GATEWAY_LABEL_WIDTH,currentY,tabWidth-GATEWAY_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // DNS1
    dns1Title = uiCreateLabel(tab,&style_section_title,DNS1_LABEL,0,currentY,DNS1_LABEL_WIDTH,LINE_HEIGHT);
    dns1Label = uiCreateLabel(tab,&style_section_text,"",DNS1_LABEL_WIDTH,currentY,tabWidth-DNS1_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // DNS2
    dns2Title = uiCreateLabel(tab,&style_section_title,DNS2_LABEL,0,currentY,DNS2_LABEL_WIDTH,LINE_HEIGHT);
    dns2Label = uiCreateLabel(tab,&style_section_text,"",DNS2_LABEL_WIDTH,currentY,tabWidth-DNS2_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Mask
    maskTitle = uiCreateLabel(tab,&style_section_title,MASK_LABEL,0,currentY,MASK_LABEL_WIDTH,LINE_HEIGHT);
    maskLabel = uiCreateLabel(tab,&style_section_text,"",MASK_LABEL_WIDTH,currentY,tabWidth-MASK_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // NTP status
    ntpTitle = uiCreateLabel(tab,&style_section_title,NTP_LABEL,0,currentY,NTP_LABEL_WIDTH,LINE_HEIGHT);
    ntpDateTitle = uiCreateLabel(tab,&style_section_title,NTP_DATE_LABEL,NTP_LABEL_WIDTH,currentY,NTP_DATE_WIDTH,LINE_HEIGHT);
    ntpDateLabel = uiCreateLabel(tab,&style_section_text,"",NTP_LABEL_WIDTH+NTP_DATE_WIDTH,currentY,tabWidth-NTP_LABEL_WIDTH-NTP_DATE_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    ntpServerTitle = uiCreateLabel(tab,&style_section_title,NTP_SRV_LABEL,NTP_LABEL_WIDTH,currentY,NTP_SRV_WIDTH,LINE_HEIGHT);
    ntpServerLabel = uiCreateLabel(tab,&style_section_text,NTP_SERVER,NTP_LABEL_WIDTH+NTP_SRV_WIDTH,currentY,tabWidth-NTP_LABEL_WIDTH-NTP_SRV_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    ntpSyncTitle = uiCreateLabel(tab,&style_section_title,NTP_SYNC_LABEL,NTP_LABEL_WIDTH,currentY,NTP_SYNC_WIDTH,LINE_HEIGHT);
    ntpSyncLabel = uiCreateLabel(tab,&style_section_text,"",NTP_LABEL_WIDTH+NTP_SYNC_WIDTH,currentY,tabWidth-NTP_LABEL_WIDTH-NTP_SYNC_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;

}

void createSdTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // SD card mounted
    currentY+=SPACER;
    sdTitle = uiCreateLabel(tab,&style_section_title,SD_LABEL,0,currentY,SD_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    sdToggle = uiCreateToggle(tab,&style_section_text,toggle_sd_cb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+SPACER;
    // Total size
    totalBytesTitle = uiCreateLabel(tab,&style_section_title,TOTAL_SIZE_LABEL,0,currentY,TOTAL_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    totalBytesLabel = uiCreateLabel(tab,&style_section_text,"",TOTAL_SIZE_LABEL_WIDTH,currentY,tabWidth-TOTAL_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Used size
    usedBytesTitle = uiCreateLabel(tab,&style_section_title,USED_SIZE_LABEL,0,currentY,USED_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    usedBytesLabel = uiCreateLabel(tab,&style_section_text,"",USED_SIZE_LABEL_WIDTH,currentY,tabWidth-USED_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // SarsatJRX folder
    logFolderTitle = uiCreateLabel(tab,&style_section_title,LOG_FOLDER_LABEL,0,currentY,LOG_FOLDER_LABEL_WIDTH,LINE_HEIGHT);
    logFolderLabel = uiCreateLabel(tab,&style_section_text,"",LOG_FOLDER_LABEL_WIDTH,currentY,tabWidth-LOG_FOLDER_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Beacons
    beaconsTitle = uiCreateLabel(tab,&style_section_title,BEACONS_LABEL,0,currentY,BEACONS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    beaconsList = lv_list_create(tab); 
    lv_obj_set_pos(beaconsList,0,currentY);
    int bListHeight = tabHeight-currentY;
    lv_obj_set_size(beaconsList, BEACON_LIST_WIDTH, bListHeight);
    //lv_obj_set_style_pad_row(beaconsList, 2, 0);
    // Up
    beaconsUpButton = uiCreateImageButton(tab,LV_SYMBOL_UP,beacon_up_cb,LV_EVENT_ALL,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X1,currentY);
    // Down
    int bottomButtonY = currentY+bListHeight-BEACON_BUTTON_HEIGHT;
    beaconsDownButton = uiCreateImageButton(tab,LV_SYMBOL_DOWN,beacon_down_cb,LV_EVENT_ALL,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X1,bottomButtonY);
    // Load
    beaconsLoadButton = uiCreateImageButton(tab,LV_SYMBOL_PLAY,beacon_load_cb,LV_EVENT_CLICKED,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X2,currentY);
    // Delete
    beaconsDeleteButton = uiCreateImageButton(tab,LV_SYMBOL_TRASH,beacon_delete_cb,LV_EVENT_SHORT_CLICKED,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X2,bottomButtonY);
    lv_obj_add_event_cb(beaconsDeleteButton, beacon_delete_longpress_cb, LV_EVENT_LONG_PRESSED, NULL);
}

static void toggle_radio_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle radio :" + String(state));
    Radio* radio = Radio::getRadio();
    if(state)
    {
        radio->radioInit();
        uiSettingsUpdateView();
    }
    else
    {
        radio->radioStop();
        uiSettingsUpdateView();
    }
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioState(state);
}

static void radio_prev_freq_cb(lv_event_t * e)
{
    Radio::getRadio()->previousFrequency();
}

static void radio_next_freq_cb(lv_event_t * e)
{   
    Radio::getRadio()->nextFrequency();
}


// Hack from https://github.com/lvgl/lvgl/issues/4295#issuecomment-1603873284 to prevent full redraw of keyboard on each button press
static void invalidate_button_area(const lv_obj_t * obj, uint16_t btn_idx)
{
    if(btn_idx == LV_BTNMATRIX_BTN_NONE) return;

    lv_area_t btn_area;
    lv_area_t obj_area;

    lv_btnmatrix_t * btnm = (lv_btnmatrix_t *)obj;;
    if(btn_idx >= btnm->btn_cnt) return;

    lv_area_copy(&btn_area, &btnm->button_areas[btn_idx]);
    lv_obj_get_coords(obj, &obj_area);

    /*The buttons might have outline and shadow so make the invalidation larger with the gaps between the buttons.
     *It assumes that the outline or shadow is smaller than the gaps*/
    lv_coord_t row_gap = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    lv_coord_t col_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    /*Be sure to have a minimal extra space if row/col_gap is small*/
    lv_coord_t dpi = lv_disp_get_dpi(lv_obj_get_disp(obj));
    row_gap = LV_MAX(row_gap, dpi / 10);
    col_gap = LV_MAX(col_gap, dpi / 10);

    /*Convert relative coordinates to absolute*/
    btn_area.x1 += obj_area.x1 - row_gap;
    btn_area.y1 += obj_area.y1 - col_gap;
    btn_area.x2 += obj_area.x1 + row_gap;
    btn_area.y2 += obj_area.y1 + col_gap;

    lv_obj_invalidate_area(obj, &btn_area);
}

void event_pre_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED) {
        lv_disp_enable_invalidation(NULL, false); //Disable invalidation on press
    }
}

static void radio_keyboard_cb(lv_event_t * e);
void event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_keyboard_t * keyboard = (lv_keyboard_t *) obj;
    if(code == LV_EVENT_PRESSED) {
        //Invalidate only the button
        lv_disp_enable_invalidation(NULL, true);
        invalidate_button_area(obj, keyboard->btnm.btn_id_sel);
        lv_disp_enable_invalidation(NULL, false);
    }
    if(code == LV_EVENT_RELEASED) {
        lv_disp_enable_invalidation(NULL, true);
        invalidate_button_area(obj, keyboard->btnm.btn_id_sel); //Enable invalidation on release
        lv_obj_invalidate(lv_textarea_get_label(lv_keyboard_get_textarea(obj)));
    }
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_disp_enable_invalidation(NULL, true);
        radio_keyboard_cb(e);
    }
}
// End hack

void startEditFreq()
{   // Show text area
    lv_obj_clear_flag(radioFreqTextArea, LV_OBJ_FLAG_HIDDEN);
    // Create a new Keyboard
    radioFreqKeyboard = lv_keyboard_create(radioTab);
    lv_obj_set_size(radioFreqKeyboard,  radioTabWidth, radioTabHeight-RADIO_FREQ_HEIGHT-4);
    lv_keyboard_set_textarea(radioFreqKeyboard, radioFreqTextArea);
    lv_keyboard_set_mode(radioFreqKeyboard, LV_KEYBOARD_MODE_NUMBER);
    // Hack (see above)
    lv_obj_add_event_cb(radioFreqKeyboard, event_pre_cb,  (lv_event_code_t)(LV_EVENT_ALL | LV_EVENT_PREPROCESS), NULL);
    lv_obj_add_event_cb(radioFreqKeyboard, event_cb, LV_EVENT_ALL, NULL);
    // End hack
    //lv_obj_add_event_cb(radioFreqKeyboard, radio_keyboard_cb, LV_EVENT_ALL, NULL);

    // Prepare text area content
    char buffer[16];
    sprintf(buffer,"%3.4f",Radio::getRadio()->getFrequency());
    lv_textarea_set_text(radioFreqTextArea, "");
    lv_textarea_set_placeholder_text(radioFreqTextArea, buffer);
}

void stopEditFreq()
{   // Hide text area and delete keyboard
    lv_obj_add_flag(radioFreqTextArea, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del(radioFreqKeyboard);
}

static void radio_freq_cb(lv_event_t * e)
{   // Keyboard input
    startEditFreq();
}

static bool ignoreNext = false;
static String previousContent;
static void radio_freq_ta_cb(lv_event_t * e)
{
    if(radioFreqTextArea)
    {
        String content = String(lv_textarea_get_text(radioFreqTextArea));
        //Serial.printf("Input ctrl, previous = %s, content = %s, length = %d\n",previousContent.c_str(),content.c_str(),content.length());
        if (content.length() == 3)
        {
            if(content.indexOf('.')<0)
            {   // Check that we are not backspacing
                if(previousContent.length()<4)
                {
                    lv_textarea_set_cursor_pos(radioFreqTextArea, 3);
                    lv_textarea_add_char(radioFreqTextArea, '.');
                    ignoreNext = true;
                }
            }
        }
        else if (content.length() == 4)
        {
            if(content.charAt(3) == '.')
            {   // Check that we are backspacing
                if(previousContent.length()>4)
                {
                    lv_textarea_set_cursor_pos(radioFreqTextArea, 4);
                    lv_textarea_del_char(radioFreqTextArea);
                    ignoreNext = true;
                }
            }
            else if(content.indexOf('.')<0)
            {   // Add a '.' at position 3
                int oldPos = lv_textarea_get_cursor_pos(radioFreqTextArea);
                lv_textarea_set_cursor_pos(radioFreqTextArea, 3);
                lv_textarea_add_char(radioFreqTextArea,'.');
                lv_textarea_set_cursor_pos(radioFreqTextArea, oldPos+1);
                ignoreNext = true;
            }
        }
        else if (content.length() > 4)
        {
            if(content.charAt(content.length()-1) == '.')
            {   // Prevent '.' at other positions
                lv_textarea_set_cursor_pos(radioFreqTextArea, (content.length()));
                lv_textarea_del_char(radioFreqTextArea);
                ignoreNext = true;
            }
        }
        if(ignoreNext)
        {
            ignoreNext = false;
        }
        else
        {
            previousContent = content;
        }
    }
}

static void radio_keyboard_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_READY) || (code == LV_EVENT_CANCEL)) 
    {
        stopEditFreq();
        if(code == LV_EVENT_READY)
        {   // Change frequency
            const char * txt = lv_textarea_get_text(radioFreqTextArea);
            float newFreq = atof(txt);
            Serial.printf("Found frequency %3.4f\n",newFreq);
            Radio::getRadio()->setFrequency(newFreq);
        }
    }
}

static void toggle_filter1_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioFilter1Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter1(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter1(state);
}

static void toggle_filter2_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioFilter2Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter2(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter2(state);
}

static void toggle_filter3_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioFilter3Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter3(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter3(state);
}

void createRadioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Radion on/off
    currentY+=SPACER;
    radioToggle = uiCreateToggle(tab,&style_section_text,toggle_radio_cb,RADIO_TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Previous button
    freqPrevButton = uiCreateImageButton(tab,LV_SYMBOL_LEFT,radio_prev_freq_cb,LV_EVENT_CLICKED,RADIO_BUTTONS_WIDTH, RADIO_FREQ_HEIGHT,RADIO_FREQ_NEXT_X,0);
    // Freq
    radiioFreqLabelButton = uiCreateLabelButton(tab,"000.0000 MHz",radio_freq_cb,LV_EVENT_CLICKED,lv_color_make(10, 10, 10),tabWidth-RADIO_FREQ_X-RADIO_BUTTONS_WIDTH-8, RADIO_FREQ_HEIGHT,RADIO_FREQ_X,0);
    lv_obj_add_style(radiioFreqLabelButton,&style_text_lcd_large,0);
    // Freq text area
    radioFreqTextArea = uiCreateTextArea(tab,radio_freq_ta_cb,tabWidth-RADIO_FREQ_X-RADIO_BUTTONS_WIDTH-8, RADIO_FREQ_HEIGHT,RADIO_FREQ_X,0);
    lv_obj_add_style(radioFreqTextArea,&style_header,0);
    lv_obj_set_style_pad_all(radioFreqTextArea,4,0);
    // Hide it for now
    lv_obj_add_flag(radioFreqTextArea, LV_OBJ_FLAG_HIDDEN);
    // Next button
    freqNextButton = uiCreateImageButton(tab,LV_SYMBOL_RIGHT,radio_next_freq_cb,LV_EVENT_CLICKED,RADIO_BUTTONS_WIDTH, RADIO_FREQ_HEIGHT,tabWidth-RADIO_BUTTONS_WIDTH-4,0);
    currentY+=(TOGGLE_LINE_HEIGHT+SPACER+HALF_SPACER);
    // Meter
    radioMeter = lv_bar_create(tab);
    lv_obj_set_size(radioMeter, tabWidth-4, RADIO_METER_HEIGHT);
    lv_obj_set_pos(radioMeter,0,currentY);
    lv_obj_add_style(radioMeter, &style_meter, LV_PART_INDICATOR);
    lv_bar_set_range(radioMeter, 0, 255);
    lv_obj_set_style_anim_time(radioMeter,200,LV_PART_MAIN);
    currentY += (RADIO_METER_HEIGHT+HALF_SPACER);
    // Version
    radioVersionTitle = uiCreateLabel(tab,&style_section_title,VERSION_LABEL,0,currentY,VERSION_LABEL_WIDTH,LINE_HEIGHT);
    radioVersionLabel = uiCreateLabel(tab,&style_section_text,Radio::getRadio()->getVersion().c_str(),VERSION_LABEL_WIDTH,currentY,tabWidth-VERSION_LABEL_WIDTH,LINE_HEIGHT);
    currentY += (LINE_HEIGHT+HALF_SPACER);
    // Filter toggles
    int currentX = 0;
    radioFiltersTitle = uiCreateLabel(tab,&style_section_title,FILTERS_LABEL,0,currentY+HALF_SPACER,FILTERS_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTERS_LABEL_WIDTH+SPACER;
    radioFilter1Toggle = uiCreateToggle(tab,&style_section_text,toggle_filter1_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    radioFilter1Label = uiCreateLabel(tab,&style_section_text,FILTER1_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    radioFilter2Toggle = uiCreateToggle(tab,&style_section_text,toggle_filter2_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    radioFilter2Label = uiCreateLabel(tab,&style_section_text,FILTER2_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    radioFilter3Toggle = uiCreateToggle(tab,&style_section_text,toggle_filter3_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    radioFilter3Label = uiCreateLabel(tab,&style_section_text,FILTER3_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    //currentX+=FILTER_LABEL_WIDTH+SPACER;
    currentY+=(TOGGLE_LINE_HEIGHT+SPACER+SPACER);

}

void uiSettingsCreateView(lv_obj_t * cont)
{   // Tab view
    settingsTabview = lv_tabview_create(cont, LV_DIR_LEFT, 50);
    // Hide tab view for now
    lv_obj_add_flag(settingsTabview, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(settingsTabview,uiBackgroundColor,0);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(settingsTabview);
    //lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    //lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);

    /*Add 5 tabs */
    lv_obj_t * tab1 = lv_tabview_add_tab(settingsTabview, "Sys.");
    lv_obj_t * tab2 = lv_tabview_add_tab(settingsTabview, "Wifi");
    lv_obj_t * tab3 = lv_tabview_add_tab(settingsTabview, "Net.");
    lv_obj_t * tab4 = lv_tabview_add_tab(settingsTabview, "SD");
    radioTab = lv_tabview_add_tab(settingsTabview, "Radio");
    lv_obj_add_style(tab1, &style_pad_small, 0);
    lv_obj_add_style(tab2, &style_pad_small, 0);
    lv_obj_add_style(tab3, &style_pad_small, 0);
    lv_obj_add_style(tab4, &style_pad_small, 0);
    lv_obj_add_style(radioTab, &style_pad_small, 0);
    int tabWidth = lv_obj_get_width(tab1) - 8; // 2*4 px padding
    int tabHeight = lv_obj_get_height(tab1) - 8 - FOOTER_HEIGHT; // 2*4 px padding
    radioTabWidth = tabWidth;
    radioTabHeight = tabHeight;

    //lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    //lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    // Add content to the tabs
    int currentY = 0;
    createSystemTab(tab1,currentY,tabWidth);
    createWifiTab(tab2,currentY,tabWidth);
    createNetworkTab(tab3,currentY,tabWidth);
    createSdTab(tab4,currentY,tabWidth,tabHeight);
    createRadioTab(radioTab,currentY,tabWidth,tabHeight);
    lv_obj_clear_flag(lv_tabview_get_content(settingsTabview), LV_OBJ_FLAG_SCROLLABLE);
}

void uiSettingsUpdateView()
{
    Hardware* hardware = Hardware::getHardware();
    Settings* settings = hardware->getSettings();
    // System tab
    // Vbat
    lv_label_set_text(vBatLabel,hardware->getVccStringValue().c_str());
    // Ram
    lv_label_set_text(ramSizeLabel,formatMemoryValue(ESP.getHeapSize(),true).c_str());
    lv_label_set_text(ramFreeLabel,formatMemoryValue(ESP.getFreeHeap(),true).c_str());
    // PS Ram
    lv_label_set_text(psRamSizeLabel,formatMemoryValue(ESP.getPsramSize(),false).c_str());
    lv_label_set_text(psRamFreeLabel,formatMemoryValue(ESP.getFreePsram(),false).c_str());
    // Flash
    lv_label_set_text(flashSizeLabel,formatMemoryValue(ESP.getFlashChipSize(),false).c_str());
    lv_label_set_text(flashFreqLabel,formatHzFrequencyValue(ESP.getFlashChipSpeed()).c_str());

    uiSettingsUpdateWifi();

    // SD tab
    Filesystems *filesystems = hardware->getFilesystems();
    // Clean list
    lv_obj_clean(beaconsList);
    beaconCount = 0;
    currentBeacon = NULL;
    if(filesystems->isSdFilesystemMounted())
    {   // SD toogle on
        lv_obj_add_state(sdToggle, LV_STATE_CHECKED);
        FS* sdfs = filesystems->getSdFilesystem();
        File logDir = filesystems->getLogDir();
        if(logDir && logDir.isDirectory())
        {
            File beacon = logDir.openNextFile();
            lv_obj_t * btn;
            char buffer[32];
            while(beacon)
            {
                if(!beacon.isDirectory())
                {
                    String name = beacon.name();
                    if(name.endsWith(LOG_FILE_EXTENSION))
                    {
                        formatBeaconFileName(buffer,name);
                        lv_obj_t *lab = lv_label_create(beaconsList);
                        // Reverse list order to have latest beacons at the top of the list
                        lv_obj_move_background(lab);
                        lv_obj_set_user_data(lab,new String(name));
                        lv_obj_add_event_cb(lab, beacon_clicked_cb, LV_EVENT_CLICKED, NULL);
                        lv_obj_add_style(lab,&style_section_text,0);
                        lv_obj_set_style_text_color(lab,uiOkColor,LV_STATE_CHECKED);
                        lv_obj_add_flag(lab,LV_OBJ_FLAG_CLICKABLE);
                        lv_label_set_text(lab, buffer);
                        beaconCount++;
                    }
                }
                beacon = logDir.openNextFile();
            }
            if(beaconCount > 0)
            {   // Try last selected item
                currentBeacon = lv_obj_get_child(beaconsList, lastBeaconIndex);
                if(!currentBeacon)
                {   // Else first item
                    currentBeacon = lv_obj_get_child(beaconsList, 0);
                }
                if(currentBeacon)
                {
                    lv_obj_add_state(currentBeacon, LV_STATE_CHECKED);
                    lv_obj_scroll_to_view(currentBeacon, LV_ANIM_OFF);
                }
            }
        }
    }
    else
    {   // Wifi toggle on
        lv_obj_clear_state(sdToggle, LV_STATE_CHECKED);
    }
    // total size
    lv_label_set_text(totalBytesLabel,formatMemoryValue((uint32_t)filesystems->getSdTotalBytes(),true).c_str());
    // total size
    lv_label_set_text(usedBytesLabel,formatMemoryValue((uint32_t)filesystems->getSdUsedBytes(),true).c_str());
    // Update log folder
    uiUpdateLogFolder();
}

void uiSettingsUpdateWifi()
{   // Wifi tab
    // Wifi state and Portal mode
    WifiStatus status = wifiManagerGetStatus();
    if(status == WifiStatus::OFF)
    {   // Wifi toggle off
        lv_obj_clear_state(wifiToggle, LV_STATE_CHECKED);
        // Disable portal toggle
        lv_obj_add_state(portalToggle, LV_STATE_DISABLED);
        lv_obj_clear_state(portalToggle, LV_STATE_CHECKED);
    }
    else
    {   // Wifi toggle on
        lv_obj_add_state(wifiToggle, LV_STATE_CHECKED);
        // Enable portal toggle
        lv_obj_clear_state(portalToggle, LV_STATE_DISABLED);
        if((status == WifiStatus::PORTAL) || (status == WifiStatus::PORTAL_CONNECTED))
        {
            lv_obj_add_state(portalToggle, LV_STATE_CHECKED);
        }
        else
        {
            lv_obj_clear_state(portalToggle, LV_STATE_CHECKED);
        }
    }

    // Mode (Station / Acess Point)
    lv_label_set_text(modeLabel,wifiManagerGetMode());
    // Status (Connected etc)
    lv_label_set_text(statusLabel,wifiManagerGetStatusString().c_str());
    lv_obj_set_style_text_color(statusLabel, (wifiManagerIsConnected() ? uiOkColor : uiKoColor),0);
    // Signal (RSSI)
    lv_label_set_text(signalLabel,formatDbmValue(WiFi.RSSI()).c_str());
    // SSID
    lv_label_set_text(ssidLabel,String(WiFi.SSID()).c_str());
    // IP @
    String ipString = WiFi.localIP().toString();
    lv_label_set_text(ipLabel,ipString.c_str());
    // Gateway IP
    String gwString = WiFi.gatewayIP().toString();
    lv_label_set_text(gatewayLabel,gwString.c_str());
    // DNS
    String dnsString = WiFi.dnsIP(0).toString();
    lv_label_set_text(dnsLabel,dnsString.c_str());

    // Net tab
    lv_label_set_text(netStatusLabel,wifiManagerGetStatusString().c_str());
    lv_obj_set_style_text_color(netStatusLabel, (wifiManagerIsConnected() ? uiOkColor : uiKoColor),0);
    // Mac @
    lv_label_set_text(macLabel,WiFi.macAddress().c_str());
    // IP @
    lv_label_set_text(netIpLabel,ipString.c_str());
    // Gateway IP
    lv_label_set_text(netGatewayLabel,gwString.c_str());
    // DNS
    lv_label_set_text(dns1Label,dnsString.c_str());
    // DNS2
    lv_label_set_text(dns2Label,WiFi.dnsIP(0).toString().c_str());
    // Mask
    lv_label_set_text(maskLabel,WiFi.subnetMask().toString().c_str());
    // Date
    Rtc* rtc = Rtc::getRtc();
    lv_label_set_text(ntpDateLabel,(rtc->getDateString() + " - " + rtc->getTimeString()).c_str());
    // NTP
    lv_label_set_text(ntpSyncLabel,(rtc->isNtpSynched() ? "OK" : "KO"));
    lv_obj_set_style_text_color(ntpSyncLabel, (rtc->isNtpSynched() ? uiOkColor : uiKoColor),0);
}

void uiSettingsUpdateRadioStatus(bool radioStatus)
{
    if(radioStatus)
    {   // Radio toggle on
        lv_obj_add_state(radioToggle, LV_STATE_CHECKED);
        Radio * radio = Radio::getRadio();
        // Update version
        lv_label_set_text(radioVersionLabel,radio->getVersion().c_str());
        // Update filters
        radio->getFilter1() ? lv_obj_add_state(radioFilter1Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(radioFilter1Toggle, LV_STATE_CHECKED);
        radio->getFilter2() ? lv_obj_add_state(radioFilter2Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(radioFilter2Toggle, LV_STATE_CHECKED);
        radio->getFilter3() ? lv_obj_add_state(radioFilter3Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(radioFilter3Toggle, LV_STATE_CHECKED);
    }
    else
    {   // Radio toggle off
        lv_obj_clear_state(radioToggle, LV_STATE_CHECKED);
    }
}

void uiSettingsUpdatePower(int power)
{
    lv_bar_set_value(radioMeter, power, LV_ANIM_ON);
}

void uiSettingsUpdateFreq(char* freqBuffer, bool scanOn)
{
    lv_label_set_text(radiioFreqLabelButton,freqBuffer);
    lv_obj_set_style_text_color(radiioFreqLabelButton,scanOn ? uiOnColor : uiOffColor,0);
}




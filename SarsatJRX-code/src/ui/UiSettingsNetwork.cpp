#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <WiFi.h>
#include <WifiManager.h>
#include <Settings.h>

// Wifi / Net
#define WIFI_LABEL          "Wifi:"
#define WIFI_LABEL_WIDTH    80
#define WIFI_TOGGLE_X       120
// Wifi Config
#define WIFI_CONFIG_LABEL               "Config:"
#define WIFI_CONFIG_LABEL_WIDTH         60
#define WIFI_CONFIG_SSID_LABEL          "- Ssid:"
#define WIFI_CONFIG_SSID_LABEL_WIDTH    54
#define WIFI_CONFIG_PSK_LABEL           "- Psk:"
// Wifi setup
#define MODE_LABEL          "Mode:"
#define MODE_LABEL_WIDTH    64
#define STATUS_LABEL        "Status:"
#define STATUS_LABEL_WIDTH  64
#define SIGNAL_LABEL        "Signal:"
#define SIGNAL_LABEL_WIDTH  64
#define SSID_LABEL          "SSID:"
#define SSID_LABEL_WIDTH    64
#define MAC_LABEL           "MAC:"
#define MAC_LABEL_WIDTH     64
#define IP_LABEL            "IP:"
#define IP_LABEL_WIDTH      64
#define GATEWAY_LABEL       "Gtway:"
#define GATEWAY_LABEL_WIDTH 64
#define DNS_LABEL           "DNS:"
#define DNS1_LABEL          "DNS 1:"
#define DNS1_LABEL_WIDTH    64
#define DNS2_LABEL          "DNS 2:"
#define DNS2_LABEL_WIDTH    64
#define MASK_LABEL          "Mask:"
#define MASK_LABEL_WIDTH    64
// NTP status
#define NTP_LABEL           "NTP:"
#define NTP_LABEL_WIDTH     50
#define NTP_DATE_LABEL      "- Date:"
#define NTP_DATE_WIDTH      70
#define NTP_SRV_LABEL       "- Server:"
#define NTP_SRV_WIDTH       70
#define NTP_SYNC_LABEL      "- Sync.:"
#define NTP_SYNC_WIDTH      70

// Wifi
static lv_obj_t * wifiTitle;
static lv_obj_t * wifiToggle;
static lv_obj_t * wifiConfigTitle;
static lv_obj_t * wifiConfigSsidTitle;
static lv_obj_t * wifiConfigSsidLabel;
static lv_obj_t * wifiConfigPskTitle;
static lv_obj_t * wifiConfigPskLabel;
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
static lv_obj_t * dns2Title;
static lv_obj_t * dns2Label;
static lv_obj_t * gatewayTitle;
static lv_obj_t * gatewayLabel;
static lv_obj_t * macTitle;
static lv_obj_t * macLabel;
static lv_obj_t * maskTitle;
static lv_obj_t * maskLabel;
static lv_obj_t * ntpTitle;
static lv_obj_t * ntpDateTitle;
static lv_obj_t * ntpDateLabel;
static lv_obj_t * ntpServerTitle;
static lv_obj_t * ntpServerLabel;
static lv_obj_t * ntpSyncTitle;
static lv_obj_t * ntpSyncLabel;

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

void createWifiTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Wifi On/Off -> save to EEProm
    currentY+=2;
    wifiTitle = uiCreateLabel(tab,&style_section_title,WIFI_LABEL,0,currentY,WIFI_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    wifiToggle = uiCreateToggle(tab,&style_section_text,toggle_wifi_cb,WIFI_TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+HALF_SPACER;
    // Wifi config
    wifiConfigTitle = uiCreateLabel(tab,&style_section_title,WIFI_CONFIG_LABEL,0,currentY,WIFI_CONFIG_LABEL_WIDTH,LINE_HEIGHT);
    wifiConfigSsidTitle = uiCreateLabel(tab,&style_section_title,WIFI_CONFIG_SSID_LABEL,WIFI_CONFIG_LABEL_WIDTH,currentY,WIFI_CONFIG_SSID_LABEL_WIDTH,LINE_HEIGHT);
    wifiConfigSsidLabel = uiCreateLabel(tab,&style_section_text,"",WIFI_CONFIG_LABEL_WIDTH+WIFI_CONFIG_SSID_LABEL_WIDTH,currentY,tabWidth-WIFI_CONFIG_LABEL_WIDTH-WIFI_CONFIG_SSID_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    wifiConfigPskTitle = uiCreateLabel(tab,&style_section_title,WIFI_CONFIG_PSK_LABEL,WIFI_CONFIG_LABEL_WIDTH,currentY,WIFI_CONFIG_SSID_LABEL_WIDTH,LINE_HEIGHT);
    wifiConfigPskLabel = uiCreateLabel(tab,&style_section_text,"",WIFI_CONFIG_LABEL_WIDTH+WIFI_CONFIG_SSID_LABEL_WIDTH,currentY,tabWidth-WIFI_CONFIG_LABEL_WIDTH-WIFI_CONFIG_SSID_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT+HALF_SPACER;
    // Status (Connected etc)
    statusTitle = uiCreateLabel(tab,&style_section_title,STATUS_LABEL,0,currentY,STATUS_LABEL_WIDTH,LINE_HEIGHT);
    statusLabel = uiCreateLabel(tab,&style_section_text,"",STATUS_LABEL_WIDTH,currentY,tabWidth-STATUS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // SSID
    ssidTitle = uiCreateLabel(tab,&style_section_title,SSID_LABEL,0,currentY,SSID_LABEL_WIDTH,LINE_HEIGHT);
    ssidLabel = uiCreateLabel(tab,&style_section_text,"",SSID_LABEL_WIDTH,currentY,tabWidth-SSID_LABEL_WIDTH,LINE_HEIGHT);
    // Mode (Station / Acess Point)
    modeTitle = uiCreateLabel(tab,&style_section_title,MODE_LABEL,SEC_COL_X,currentY,MODE_LABEL_WIDTH,LINE_HEIGHT);
    modeLabel = uiCreateLabel(tab,&style_section_text,"",SEC_COL_X+MODE_LABEL_WIDTH,currentY,tabWidth-SEC_COL_X-MODE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Signal (RSSI)
    signalTitle = uiCreateLabel(tab,&style_section_title,SIGNAL_LABEL,0,currentY,SIGNAL_LABEL_WIDTH,LINE_HEIGHT);
    signalLabel = uiCreateLabel(tab,&style_section_text,"",SIGNAL_LABEL_WIDTH,currentY,tabWidth-SIGNAL_LABEL_WIDTH,LINE_HEIGHT);
    // DNS 1
    dnsTitle = uiCreateLabel(tab,&style_section_title,DNS1_LABEL,SEC_COL_X,currentY,DNS1_LABEL_WIDTH,LINE_HEIGHT);
    dnsLabel = uiCreateLabel(tab,&style_section_text,"",SEC_COL_X+DNS1_LABEL_WIDTH,currentY,tabWidth-SEC_COL_X-DNS1_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // IP @
    ipTitle = uiCreateLabel(tab,&style_section_title,IP_LABEL,0,currentY,IP_LABEL_WIDTH,LINE_HEIGHT);
    ipLabel = uiCreateLabel(tab,&style_section_text,"",IP_LABEL_WIDTH,currentY,tabWidth-IP_LABEL_WIDTH,LINE_HEIGHT);
    // DNS 2
    dns2Title = uiCreateLabel(tab,&style_section_title,DNS2_LABEL,SEC_COL_X,currentY,DNS2_LABEL_WIDTH,LINE_HEIGHT);
    dns2Label = uiCreateLabel(tab,&style_section_text,"",SEC_COL_X+DNS2_LABEL_WIDTH,currentY,tabWidth-SEC_COL_X-DNS2_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Gateway IP
    gatewayTitle = uiCreateLabel(tab,&style_section_title,GATEWAY_LABEL,0,currentY,GATEWAY_LABEL_WIDTH,LINE_HEIGHT);
    gatewayLabel = uiCreateLabel(tab,&style_section_text,"",GATEWAY_LABEL_WIDTH,currentY,tabWidth-GATEWAY_LABEL_WIDTH,LINE_HEIGHT);
    // Mask
    maskTitle = uiCreateLabel(tab,&style_section_title,MASK_LABEL,SEC_COL_X,currentY,MASK_LABEL_WIDTH,LINE_HEIGHT);
    maskLabel = uiCreateLabel(tab,&style_section_text,"",SEC_COL_X+MASK_LABEL_WIDTH,currentY,tabWidth-SEC_COL_X-MASK_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Mac @
    macTitle = uiCreateLabel(tab,&style_section_title,MAC_LABEL,0,currentY,MAC_LABEL_WIDTH,LINE_HEIGHT);
    macLabel = uiCreateLabel(tab,&style_section_text,"",MAC_LABEL_WIDTH,currentY,tabWidth-MAC_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // NTP status
    ntpTitle = uiCreateLabel(tab,&style_section_title,NTP_LABEL,0,currentY,NTP_LABEL_WIDTH,LINE_HEIGHT);
    ntpDateTitle = uiCreateLabel(tab,&style_section_title,NTP_DATE_LABEL,NTP_LABEL_WIDTH,currentY,NTP_DATE_WIDTH,LINE_HEIGHT);
    ntpDateLabel = uiCreateLabel(tab,&style_section_text,"",NTP_LABEL_WIDTH+NTP_DATE_WIDTH,currentY,tabWidth-NTP_LABEL_WIDTH-NTP_DATE_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    ntpServerTitle = uiCreateLabel(tab,&style_section_title,NTP_SRV_LABEL,NTP_LABEL_WIDTH,currentY,NTP_SRV_WIDTH,LINE_HEIGHT);
    ntpServerLabel = uiCreateLabel(tab,&style_section_text,NTP_SERVER1,NTP_LABEL_WIDTH+NTP_SRV_WIDTH,currentY,tabWidth-NTP_LABEL_WIDTH-NTP_SRV_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    ntpSyncTitle = uiCreateLabel(tab,&style_section_title,NTP_SYNC_LABEL,NTP_LABEL_WIDTH,currentY,NTP_SYNC_WIDTH,LINE_HEIGHT);
    ntpSyncLabel = uiCreateLabel(tab,&style_section_text,"",NTP_LABEL_WIDTH+NTP_SYNC_WIDTH,currentY,tabWidth-NTP_LABEL_WIDTH-NTP_SYNC_WIDTH,LINE_HEIGHT);
}

void uiSettingsUpdateWifi()
{   // Wifi tab
    // Wifi state
    WifiStatus status = wifiManagerGetStatus();
    if(status == WifiStatus::OFF)
    {   // Wifi toggle off
        lv_obj_clear_state(wifiToggle, LV_STATE_CHECKED);
    }
    else
    {   // Wifi toggle on
        lv_obj_add_state(wifiToggle, LV_STATE_CHECKED);
    }

    Settings* settings = Settings::getSettings();
    // Wifi config
    lv_label_set_text(wifiConfigSsidLabel,settings->getWifiSsid().c_str());
    lv_label_set_text(wifiConfigPskLabel,settings->getWifiPassPhrase().c_str());

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

    // Mac @
    lv_label_set_text(macLabel,WiFi.macAddress().c_str());
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


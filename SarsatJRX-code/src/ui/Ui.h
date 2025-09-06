#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <SarsatJRXConf.h>
#include <WifiManager.h>
#include <Beacon.h>
#include <Power.h>
#include <lvgl.h>

#define LINE_HEIGHT         18
#define HEADER_HEIGHT       30
#define FOOTER_HEIGHT       46

// Additionnal Symbols
#define SYMBOL_WIFI_CONNECTED       "\xEF\x87\xAB"
#define SYMBOL_WIFI_AP              "\xEE\x8E\xB2"
#define SYMBOL_WIFI_AP_CONNECTED    "\xEE\x94\x91"
#define SYMBOL_DISCRI               "\xEF\xA0\xBE"
#define SYMBOL_SETTINGS             "\xEF\x80\x93"
#define SYMBOL_AUDIO                "\xEF\x80\xA8"
#define SYMBOL_DISPLAY              "\xEE\x85\xA3"
#define SYMBOL_NETWORK              "\xEF\x9B\xBF"
#define SYMBOL_BLUETOOTH            "\xEF\x8A\x93"
#define SYMBOL_SD                   "\xEF\x9F\x82"
#define SYMBOL_SYSTEM               "\xEF\x81\x9A"
#define SYMBOL_SLIDERS              "\xEF\x87\x9E"
#define SYMBOL_WRENCH               "\xEF\x82\xAD"
#define SYMBOL_BELL                 "\xEF\x83\xB3"
#define SYMBOL_TOWER_CELL           "\xEE\x96\x85"
#define SYMBOL_RADIO                "\xEF\x94\x99"
 
extern const lv_font_t casscadia_mono;
extern const lv_font_t casscadia_mono_16;
extern const lv_font_t additional_symbols;
extern const lv_font_t lcd_44;

extern lv_style_t style_text_mono;
extern lv_style_t style_text_lcd_large;
extern lv_style_t style_title;
extern lv_style_t style_footer;
extern lv_style_t style_footer_highlight;
extern lv_style_t style_pad_small;
extern lv_style_t style_pad_tiny;
extern lv_style_t style_pad_none;
extern lv_style_t style_tag;
extern lv_style_t style_header;
extern lv_style_t style_section_title;
extern lv_style_t style_section_text;
extern lv_style_t style_section_toggle;
extern lv_style_t style_footer_text;
extern lv_style_t style_time;
extern lv_style_t style_meter;

extern lv_color_t uiBackgroundColor;
extern lv_color_t uiOkColor;
extern lv_color_t uiKoColor;
extern lv_color_t uiOnColor;
extern lv_color_t uiOffColor;


extern const lv_font_t * font_large;
extern const lv_font_t * font_medium;
extern const lv_font_t * font_normal;
extern const lv_font_t * font_mono;
extern const lv_font_t * font_mono_medium;

extern lv_obj_t * beaconTabview;
extern lv_obj_t * beaconMainBloc;
extern lv_obj_t * settingsTabview;

// Screens
enum class UiScreen { START, BEACON, SETTINGS};

// Methods
void createUi();

void uiSetBeacon(Beacon* beacon, int curPage, int pageCount);

void uiShowScreen(UiScreen screen);

void uiUpdateTime();

void uiUpdateTicker();

#ifdef WIFI
void uiUpdateWifiStatus();
#endif

void uiUpdateSdCardStatus();

void uiUpdateDiscritatus();

void uiUpdatePower();

void uiShowFrameReceived(bool show);

void uiSetLedSig1State(bool on);

void uiSetLedSig2State(bool on);

void uiSetLedInFrameState(bool on, bool error);

void uiSetLedFrameReceivedState(bool on);

void uiUpdateAudioPower();

void uiSetFreq(float freq, bool scanOn);

lv_obj_t * uiCreateLabel(lv_obj_t * parent, lv_style_t * style, const char* text, int x, int y, int width, int height);

lv_obj_t * uiCreateToggle(lv_obj_t * parent, lv_style_t * style, lv_event_cb_t event_cb, int x, int y, int width, int height);

lv_obj_t * uiCreateImageButton(lv_obj_t * parent, const void* src, lv_event_cb_t event_cb, lv_event_code_t filter, int width, int height, int x = -1, int y = -1);

lv_obj_t * uiCreateLabelButton(lv_obj_t * parent, const char* text, lv_event_cb_t event_cb, lv_event_code_t filter, int width, int height, int x = -1, int y = -1);

lv_obj_t * uiCreateTextArea(lv_obj_t * parent, lv_event_cb_t event_cb, int width, int height, int x, int y);

void uiShowScreenSaverDialog();

void uiSettingsUpdateDateAndTime();


#endif 
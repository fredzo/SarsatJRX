#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <SarsatJRXConf.h>
#include <WifiManager.h>
#include <Beacon.h>
#include <lvgl.h>

#define LINE_HEIGHT         18
#define HEADER_HEIGHT       30
#define FOOTER_HEIGHT       46


extern const lv_font_t casscadia_mono;
extern const lv_font_t casscadia_mono_16;
extern const lv_font_t additional_symbols;
extern const lv_font_t lcd_28;

extern lv_style_t style_text_mono;
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

extern lv_color_t uiBackgroundColor;
extern lv_color_t uiOkColor;
extern lv_color_t uiKoColor;


extern const lv_font_t * font_large;
extern const lv_font_t * font_medium;
extern const lv_font_t * font_normal;
extern const lv_font_t * font_mono;

extern lv_obj_t * beaconTabview;
extern lv_obj_t * beaconMainBloc;
extern lv_obj_t * settingsTabview;

// Screens
enum class UiScreen { START, BEACON, SETTINGS};

// Methods
void createUi();

void uiSetBeacon(Beacon* beacon, int curPage, int pageCount);

void uiShowScreen(UiScreen screen);

void uiSetTime(const char* time);

#ifdef WIFI
void uiSetWifiStatus(WifiStatus status);
#endif

void uiSetSdCardStatus(bool mounted);

void uiSetPower(const char* time);

void uiShowFrameReceived(bool show);

void uiSetLedSig1State(bool on);

void uiSetLedSig2State(bool on);

void uiSetLedInFrameState(bool on);

void uiSetLedFrameReceivedState(bool on);

void uiSetPower(int power);

void uiSetFreq(float freq, bool scanOn);

lv_obj_t * uiCreateLabel(lv_obj_t * parent, lv_style_t * style, const char* text, int x, int y, int width, int height);

lv_obj_t * uiCreateToggle(lv_obj_t * parent, lv_style_t * style, lv_event_cb_t event_cb, int x, int y, int width, int height);

lv_obj_t * uiCreateImageButton(lv_obj_t * parent, const void* src, lv_event_cb_t event_cb, lv_event_code_t filter, int width, int height, int x = -1, int y = -1);

lv_obj_t * uiCreateLabelButton(lv_obj_t * parent, const char* text, lv_event_cb_t event_cb, lv_event_code_t filter, lv_color_t buttonColor, int width, int height, int x = -1, int y = -1);

#endif 
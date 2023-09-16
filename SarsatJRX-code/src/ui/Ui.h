#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <SarsatJRXConf.h>
#include <WifiManager.h>
#include <Beacon.h>
#include <lvgl.h>

extern const lv_font_t casscadia_mono;
extern const lv_font_t casscadia_mono_16;
extern const lv_font_t additional_symbols;

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
extern lv_style_t style_footer_text;
extern lv_style_t style_time;

extern lv_color_t uiBackgroundColor;

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

lv_obj_t * uiCreateLabel(lv_obj_t * parent, lv_style_t * style, const char* text, int x, int y, int width, int height);

#endif 
#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Beacon.h>
#include <lvgl.h>

// Enable serial out
#define SERIAL_OUT


extern const lv_font_t cascadica_mono;

static lv_style_t style_text_mono;
static lv_style_t style_title;
static lv_style_t style_footer;
static lv_style_t style_footer_highlight;
static lv_style_t style_icon;
static lv_style_t style_bullet;
static lv_style_t style_pad_small;
static lv_style_t style_pad_tiny;
static lv_style_t style_pad_none;
static lv_style_t style_tag;
static lv_style_t style_section_title;
static lv_style_t style_section_text;
static lv_style_t style_footer_text;

static const lv_font_t * font_large = &lv_font_montserrat_24;
static const lv_font_t * font_medium = &lv_font_montserrat_18;
static const lv_font_t * font_normal = &lv_font_montserrat_12;
static const lv_font_t * font_mono = &cascadica_mono;

extern lv_obj_t * tabview;

// Methods
void createUi();

void uiSetBeacon(Beacon* beacon, int curPage, int pageCount);

void uiSetTime(const char* time);

void uiSetPower(const char* time);

void uiShowFrameReceived(bool show);

void uiSetLedSig1State(bool on);

void uiSetLedSig2State(bool on);

void uiSetLedInFrameState(bool on);

void uiSetLedFrameReceivedState(bool on);

#endif 
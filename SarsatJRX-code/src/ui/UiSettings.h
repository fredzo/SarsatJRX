#ifndef UISETTINGS_H
#define UISETTINGS_H

#include <Arduino.h>
#include <Beacon.h>
#include <lvgl.h>

#define SPACER              8
#define HALF_SPACER         4
#define TOGGLE_LINE_HEIGHT  28
#define TOGGLE_X            160
#define TOGGLE_WIDTH        50
#define SPINBOX_LINE_HEIGHT 32
#define SPINBOX_WIDTH       80


void uiSettingsCreateView(lv_obj_t * cont);

void uiSettingsUpdateView();

void uiSettingsUpdateWifi();

void uiSettingsUpdateSystem();

void uiSettingsUpdateSd();

void uiSettingsUpdateDisplay();

void uiSettingsUpdateAudioSatus();

void uiSettingsUpdateFreq(char* freqBuffer, bool scanOn);

#endif 
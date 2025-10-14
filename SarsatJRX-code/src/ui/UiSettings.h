#ifndef UISETTINGS_H
#define UISETTINGS_H

#include <Arduino.h>
#include <Beacon.h>
#include <lvgl.h>

#define SPACER              8
#define HALF_SPACER         4
#define TOGGLE_LINE_HEIGHT  28
#define TOGGLE_X            210
#define TOGGLE_WIDTH        50
#define SPINBOX_LINE_HEIGHT 32
#define SPINBOX_BUTTON_SIZE 36
#define SPINBOX_WIDTH       80
#define SPINBOX_X           254
#define BUTTON_HEIGHT       36
#define SEC_COL_X           210


void uiSettingsCreateView(lv_obj_t * cont);

void uiSettingsUpdateView();

void uiSettingsUpdateWifi();

void uiSettingsUpdateParamWifi();

void uiSettingsUpdateBluetooth();

void uiSettingsUpdateSystem();

void uiSettingsUpdateSystemPower();

void uiSettingsUpdateSystemUptime();

void uiSettingsUpdateSd();

void uiSettingsUpdateDisplay();

void uiSettingsUpdateSettings();

void uiSettingsUpdateAudio();

void uiSettingsUpdateFreq(char* freqBuffer, bool scanOn);

void uiShowBatteryDialog();

#endif 
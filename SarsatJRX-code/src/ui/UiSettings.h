#ifndef UISETTINGS_H
#define UISETTINGS_H

#include <Arduino.h>
#include <Beacon.h>
#include <lvgl.h>

void uiSettingsCreateView(lv_obj_t * cont);

void uiSettingsUpdateView();

void uiSettingsUpdateWifi();

void uiSettingsUpdateRadioStatus(bool radioStatus);

void uiSettingsUpdatePower(int power);

void uiSettingsUpdateFreq(char* freqBuffer, bool scanOn);

#endif 
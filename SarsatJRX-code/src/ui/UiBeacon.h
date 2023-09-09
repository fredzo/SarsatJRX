#ifndef UIBEACON_H
#define UIBEACON_H

#include <Arduino.h>
#include <Beacon.h>
#include <lvgl.h>

void uiBeaconSetBeacon(Beacon* beacon);

void uiBeaconCreateTabView(lv_obj_t * cont);

#endif 
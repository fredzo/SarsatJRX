#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <SarsatJRXConf.h>

#ifdef WIFI
void wifiManagerStart();

void wifiManagerPrintStatus();

void wifiManagerHandleClient();
#endif

#endif 
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <SarsatJRXConf.h>

#ifdef WIFI
void wifiManagerStart();

bool wifiUpdateStatus();

/**
 * Returns true if wifi status changed
 */
bool wifiManagerHandleClient();

bool wifiManagerIsConnected();
#endif

#endif 
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <SarsatJRXConf.h>

#ifdef WIFI

enum class WifiStatus { CONNECTED, DISCONNECTED, PORTAL, PORTAL_CONNECTED };

void wifiManagerStart();

/**
 * Returns true if wifi status changed
 */
bool wifiManagerHandleClient();

/**
 * Returns true if Wifi is connected
 */
WifiStatus wifiManagerGetStatus();

#endif

#endif 
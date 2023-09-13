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

/**
 * Returns true if Wifi is connected
 */
bool wifiManagerIsConnected();

/**
 * Returns true if portal is active
 */
bool wifiManagerIsPortalActive();
#endif

#endif 
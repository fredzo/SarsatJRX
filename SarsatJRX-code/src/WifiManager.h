#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <SarsatJRXConf.h>
#include <Beacon.h>

#ifdef WIFI

enum class WifiStatus { OFF, CONNECTED, DISCONNECTED, PORTAL, PORTAL_CONNECTED, LINKED };

String wifiManagerGetStatusString();

const char* wifiManagerGetMode();

void wifiManagerStart();

void wifiManagerStop();

/**
 * Returns true if wifi status changed
 */
bool wifiManagerHandleClient();

/**
 * Returns wifi connection status
 */
WifiStatus wifiManagerGetStatus();

/**
 * Returns true if Wifi is connected
 */
bool wifiManagerIsConnected();

void wifiManagerNtpSynched();

size_t wifiManagerClientCount();

void wifiManagerSendTickerEvent(int countdown,bool sdMounted, bool discriOn, int batPercentage, String time);

void wifiManagerSendFrameEvent(Beacon* beacon, int frameIndex, bool valid, bool filtered, bool error);

void wifiManagerSendConfigEvent();

#endif

#endif 
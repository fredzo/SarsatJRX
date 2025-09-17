#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <Arduino.h>
#include <SarsatJRXConf.h>

#ifdef BLUETOOTH

enum class BluetoothStatus { OFF, CONNECTED, DISCONNECTED };

String bluetoothManagerGetStatusString();

const char* bluetoothManagerGetMode();

void bluetoothManagerStart();

void bluetoothManagerStop();

/**
 * Returns true if bluetooth status changed
 */
bool bluetoothManagerHandleClient();

/**
 * Returns bluetooth connection status
 */
BluetoothStatus bluetoothManagerGetStatus();

/**
 * Returns true if Bluetooth is connected
 */
bool bluetoothManagerIsConnected();

#endif

#endif 
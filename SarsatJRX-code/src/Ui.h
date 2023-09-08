#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Beacon.h>

void createUi();

void uiSetBeacon(Beacon* beacon, int curPage, int pageCount);

void uiSetTime(const char* time);

void uiSetPower(const char* time);

void uiSetSpinnerVisible(bool visible);

void uiSetFooter(const char* footer);

void uiSetLedSig1State(bool on);

void uiSetLedSig2State(bool on);

void uiSetLedInFrameState(bool on);

void uiSetLedFrameReceivedState(bool on);

#endif 
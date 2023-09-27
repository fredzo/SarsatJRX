#ifndef DECODER_H
#define DECODER_H

#include <SarsatJRXConf.h>
#include <Arduino.h>
#include <Beacon.h>
#include <RTC.h>

void analyze(void);

bool getInputState();

void resetFrameReading();

bool isFrameStarted();

byte* getFrame();

byte getByteCount();

unsigned long getFrameStartTime();

bool isFrameComplete();

void setFrameComplete(bool complete);

void setFrameFromDisk(Rtc::Date date);

bool isFrameFromDisk();

Rtc::Date getDiskFrameDate();

#ifdef DEBUG_DECODE
int* getEvents();
int getEventCount();
bool* getBits();
int getBitCount();
#endif

#endif 
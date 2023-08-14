#ifndef DECODER_H
#define DECODER_H

#include <Arduino.h>
#include <Beacon.h>

// Enable decode debuging
//#define DEBUG_DECODE


void analyze(void);

void resetFrameReading();

bool isFrameStarted();

byte* getFrame();

byte getByteCount();

unsigned long getFrameStartTime();

bool isFrameComplete();

void setFrameComplete(bool complete);

#ifdef DEBUG_DECODE
int* getEvents();
int getEventCount();
#endif

#endif 
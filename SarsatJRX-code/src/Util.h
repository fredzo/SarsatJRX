#ifndef UTIL_H
#define UTIL_H

#include <SarsatJRXConf.h>
#include <Arduino.h>
#include <RTC.h>

#ifdef DEBUG_RAM
uint32_t freeRam();
void logFreeRam();
#endif


/**
 * @brief Convert the provided frame to it's Hex String representation
 * 
 * @param frame the frame to convert
 * @param withSpace true if bytes should be seperated by spaces
 * @param start start byte
 * @param end end byte
 * @return String 
 */
String toHexString(volatile byte* frame, bool withSpace, int start, int end);

/**
 * @brief Read the provided hexString and store the content in the buffer
 * 
 * @param buffer the buffer to store the converted hex bytes into (the size must match the hex string size)
 * @param hexString the hex string to read
 */
void readHexString(volatile byte* buffer, String hexString);

String formatMemoryValue(uint32_t value, bool showByteValue);

String formatFrequencyValue(uint32_t value);

String formatDbmValue(int8_t value);

String formatHzFrequencyValue(uint32_t value);

String formatSketchInformation(uint32_t size, String md5);

void formatBeaconFileName(char* buffer, size_t size, String date);

void formatFrequencyItem(char* buffer, size_t size, int index, float frequency, bool on);

Rtc::Date parseBeaconFileName(const char* fileName);

uint8_t voltageToPercent(float voltage, float voltageMin, float voltageMax);

// Helper methods for data to string and string to data conversion
int stringToInt(String stringValue, int defaultVal);

bool stringToBool(String stringValue);

uint8_t stringToUChar(String stringValue);

String intToString(int value);

String boolToString(bool value);

String ucharToString(uint8_t value);


/* Baudot code matrix */
extern char BAUDOT_CODE[64];



#endif 
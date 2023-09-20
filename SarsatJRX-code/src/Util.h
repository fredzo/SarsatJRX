#ifndef UTIL_H
#define UTIL_H

#include <SarsatJRXConf.h>
#include <Arduino.h>

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
String toHexString(byte* frame, bool withSpace, int start, int end);

/**
 * @brief Read the provided hexString and store the content in the buffer
 * 
 * @param buffer the buffer to store the converted hex bytes into (the size must match the hex string size)
 * @param hexString the hex string to read
 */
void readHexString(byte* buffer, String hexString);

String formatMemoryValue(uint32_t value, bool showByteValue);

String formatFrequencyValue(uint32_t value);

String formatDbmValue(int8_t value);

String formatHzFrequencyValue(uint32_t value);

String formatSketchInformation(uint32_t size, String md5);

/* Baudot code matrix */
extern char BAUDOT_CODE[64];



#endif 
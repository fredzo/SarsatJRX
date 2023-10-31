#ifndef SAMPLES_H
#define SAMPLES_H

#include <Arduino.h>

/**
 * @brief Read the next sample frame into the provided buffer
 * 
 * @param frameBuffer the buffer to read the sample into
 */
void readNextSample(volatile byte* frameBuffer);

#endif 
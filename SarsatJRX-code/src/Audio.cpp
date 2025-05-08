#include <Audio.h>
#include <SarsatJRXConf.h>

#define EASING_FACTOR   5 // Easing factor percentage

// Static members
Audio *Audio::audioInstance = nullptr;

void Audio::audioInit()
{
    noiseFloor = analogReadMilliVolts(AUDIO_ADC_PIN);
    Serial.print("Noise floor = ");
    Serial.println(noiseFloor);
}
void Audio::audioStop()
{
}

void Audio::handleTask()
{   // Read value
    int n;
    n = analogReadMilliVolts(AUDIO_ADC_PIN);
    n = abs((noiseFloor) - n);
    // Apply the exponential filter to smooth the raw signal
    // (implemented by a simple linear recursive exponential filter, see https://www.statistics.com/glossary/exponential-filter/
    if(n >= signalPower)
    {   // No easing for power increase
        signalPower = n;
        filterAccumulator = n*100;
    }
    else
    {
        filterAccumulator = ((100 * EASING_FACTOR * n + (100 - EASING_FACTOR) * filterAccumulator + 50)/100);
        signalPower = (filterAccumulator + 50)/100;
    }
    //Serial.println(signalPower);
}

int Audio::getSignalPower()
{
    return signalPower;
}

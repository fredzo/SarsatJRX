#include <Audio.h>
#include <SarsatJRXConf.h>

#define EASING_FACTOR   5 // Easing factor percentage

#define DISCRI_SAMPLE_PERIOD    100   // 100ms

// Static members
Audio *Audio::audioInstance = nullptr;

void Audio::audioInit()
{
    pinMode(AUDIO_ADC_PIN, ANALOG);             // Audio level pin
    pinMode(DISCRI_JACK_PIN, INPUT_PULLUP);     // Discri jack detection pin
    // Read noise floor
    noiseFloor = analogReadMilliVolts(AUDIO_ADC_PIN);
    Serial.print("Noise floor = ");
    Serial.println(noiseFloor);
}
void Audio::audioStop()
{
}

bool Audio::isDiscriInput()
{
    return discriState;
}

bool Audio::discriHasChanged()
{   // Reset changed status after this call
    bool result = discriChanged;
    discriChanged = false;
    return result;
}

void Audio::handleTask()
{   
    // Manage discri input sampling
    unsigned long now = millis();
    if(now - lastDiscriSampleTime > DISCRI_SAMPLE_PERIOD)
    {   // Time to make a new sample
        lastDiscriSampleTime = now;
        bool newValue = (digitalRead(DISCRI_JACK_PIN) != 0);
        if(newValue != discriState)
        {
            discriState = newValue;
            discriChanged = true;
            Serial.print("Discri changed to ");
            Serial.println(discriState ? "On" : "Off");
        }
    }
    
    if(!discriState)
    {   // Only read audio input value if discri jack is not plugged in
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
    }
}

int Audio::getSignalPower()
{
    return signalPower;
}

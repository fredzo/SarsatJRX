#include <Radio.h>
#include <SarsatJRXConf.h>

// Static members
Radio *Radio::radioInstance = nullptr;

void Radio::radioInit(bool autoVolume, byte volume, bool filter1, bool filter2, bool filter3, float frequency, float* scanFrequencies)
{   // Init UART1
    radioSerial = &Serial1;
    Radio::autoVolume = autoVolume;
    Radio::volume = volume;
    Radio::filter1 = filter1;
    Radio::filter2 = filter2;
    Radio::filter3 = filter3;
    // First we need to set pins
    if(!radioSerial->setPins(UART1_RX_PIN,UART1_TX_PIN))
    {
#ifdef SERIAL_OUT
        Serial.println("Could not set UART1 pins for SA818 communication");
#endif
    }
    // Serial begin is done here
    if(dra == NULL)
    {   // Prevent double instanciation in case of multiple init() calls
        dra = new DRA818(radioSerial, SA818_UHF);
    }
#ifdef DRA818_DEBUG
    dra->set_log(&Serial);
#endif
    dra->handshake_async_cb(Radio::handshakeCallback);
    dra->group_async_cb(Radio::groupCallback);
    dra->scan_async_cb(Radio::scanCallback);
    dra->volume_async_cb(Radio::volumeCallback);
    dra->filters_async_cb(Radio::filtersCallback);
    dra->tail_async_cb(Radio::tailCallback);
    dra->rssi_async_cb(Radio::rssiCallback);
    dra->version_async_cb(Radio::versionCallback);
    dra->read_group_async_cb(Radio::readGroupCallback);
    // Use sync commands for init
    on = dra->handshake();
    version = dra->version();
    dra->volume(volume);
    setScanFrequencies(scanFrequencies);
    setFrequency(frequency);
}

void Radio::rssiCallback(int rssi)
{
    radioInstance->power = rssi;
}

void Radio::readGroupCallback(DRA818::Parameters parameters)
{
    radioInstance->parameters = parameters;
    #ifdef SERIAL_OUT
        Serial.println(parameters.toString().c_str());
    #endif
}

void Radio::groupCallback(int retCode)
{
    #ifdef SERIAL_OUT
        (retCode == 0) ? Serial.println("Group command success.") : Serial.println("Group command failed.");
    #endif
}

void Radio::handshakeCallback(int retCode)
{
    radioInstance->on = (retCode == 0);
    #ifdef SERIAL_OUT
        retCode ? Serial.println("Handshake command success.") : Serial.println("Handshake command failed.");
    #endif
}

void Radio::scanCallback(int retCode)
{   // Radion module will return 0 if the frequency is busy
    radioInstance->scanFreqBusy = (retCode == 0);
    #ifdef SERIAL_OUT
        // Serial.printf("Frequency %3.4f scan : %s\n",radioInstance->radioFrequency, radioInstance->scanFreqBusy ? "busy" : "no signal");
    #endif
    if(radioInstance->scanFreqBusy) radioInstance->stopScan(); // Stop scan on busy frequencies
    if(radioInstance->scanOn)
    {
        radioInstance->scanNext();
    }
}

void Radio::volumeCallback(int retCode)
{
    #ifdef SERIAL_OUT
        (retCode == 0) ? Serial.println("Volume command success.") : Serial.println("Volume command failed.");
    #endif
}

void Radio::filtersCallback(int retCode)
{
    #ifdef SERIAL_OUT
        (retCode == 0) ? Serial.println("Filters command success.") : Serial.println("Filters command failed.");
    #endif
}

void Radio::versionCallback(String version)
{
    radioInstance->version = version;
    #ifdef SERIAL_OUT
        Serial.printf("Radio module version : %s\n",version);
    #endif
}

void Radio::tailCallback(int retCode)
{
    #ifdef SERIAL_OUT
        (retCode == 0) ? Serial.println("Tail command success.") : Serial.println("Tail command failed.");
    #endif
}

void Radio::setScanFrequencies(const float* frequencies)
{
    scanFrequencies = frequencies;
    currentScanFrequencyIndex = 0;
    if(frequencies)
    {
        radioFrequency = scanFrequencies[0];
        frequencyCount = 0;
        while(scanFrequencies[frequencyCount]!=0)
        {
            frequencyCount++;
        }
    }
}

void Radio::nextFrequency()
{
    currentScanFrequencyIndex++;
    float newFreq = scanFrequencies[currentScanFrequencyIndex];
    if(newFreq == 0)
    {
        currentScanFrequencyIndex = 0;
        newFreq = scanFrequencies[currentScanFrequencyIndex];
    }
    setFrequency(newFreq);
}

void Radio::previousFrequency()
{
    currentScanFrequencyIndex--;
    if(currentScanFrequencyIndex<0)
    {
        currentScanFrequencyIndex = frequencyCount -1;
    }
    setFrequency(scanFrequencies[currentScanFrequencyIndex]);
}

void Radio::scanNext()
{
    radioInstance->currentScanFrequencyIndex++;
    radioFrequency = scanFrequencies[currentScanFrequencyIndex];
    if(radioFrequency == 0)
    {
        currentScanFrequencyIndex = 0;
        radioFrequency = scanFrequencies[currentScanFrequencyIndex];
    }
    dra->scan_async(radioFrequency);
}

void Radio::startScan()
{
    if(scanFrequencies)
    {
        scanOn = true;
        scanNext();
    }
    else
    {
    #ifdef SERIAL_OUT
        Serial.println("Scan impossible : call setScanFrequencies() first.");
    #endif
    }
}

void Radio::stopScan()
{
    scanOn = false;
    setFrequency(radioFrequency);
}

void Radio::toggleScan()
{
    scanOn ? stopScan() : startScan();
}

float Radio::getFrequency()
{
    return radioFrequency;
}

void Radio::setFrequency(float freq)
{
    scanOn = false;
    radioFrequency = freq;
    parameters.freq_rx = freq;
    parameters.freq_tx = freq;
    parameters.squelch = radioSquelch;
    dra->group(parameters);
    // Group command resets filters => set them back
    dra->filters(filter1, filter2, filter3);
}

void Radio::setQuelch(byte squelch)
{
    if((squelch != Radio::radioSquelch)&&(squelch>=0)&&(squelch<=8))
    {
        scanOn = false;
        radioSquelch = squelch;
        // Use set frequency group command
        setFrequency(radioFrequency);
    }
}

bool Radio::isScanFrequencyBusy()
{
    return scanFreqBusy;
}

bool Radio::isRadioOn()
{
    return on;
}

bool Radio::isScanOn()
{
    return scanOn;
}

void Radio::radioStop()
{
    on = false;
    free(dra);
    dra = NULL;
}

void Radio::handleTask()
{
    if(dra) dra->async_task();
}

int Radio::getPower()
{
    if(!scanOn)
    {   // Prevent RSSI call while scanning
        if(dra) dra->rssi_async();
    }
    return power;
}

String Radio::getVersion()
{
    return version;
}

void Radio::setAutoVolume(bool on)
{
    Radio::autoVolume = on;
}

bool Radio::getAutoVolume()
{
    return autoVolume;
}

void Radio::setVolume(byte volume)
{   // value check
    if((volume != Radio::volume)&&(volume>=1)&&(volume<=8))
    {
        Radio::volume = volume;
        if(dra) dra->volume(volume);
    }
}

byte Radio::getVolume()
{
    return volume;
}

byte Radio::getSquelch()
{
    return radioSquelch;
}

void Radio::setFilter1(bool on)
{
    Radio::filter1 = on;
    if(dra) dra->filters(filter1,filter2,filter3);
}

bool Radio::getFilter1()
{
    return filter1;
}

void Radio::setFilter2(bool on)
{
    Radio::filter2 = on;
    if(dra) dra->filters(filter1,filter2,filter3);
}

bool Radio::getFilter2()
{
    return filter2;
}

void Radio::setFilter3(bool on)
{
    Radio::filter3 = on;
    if(dra) dra->filters(filter1,filter2,filter3);
}

bool Radio::getFilter3()
{
    return filter3;
}

int Radio::getFrequencyCount()
{
    return frequencyCount;
}
float Radio::getFrequency(int number)
{
    if(number<0||number>=frequencyCount) return -1;
    return scanFrequencies[number];
}

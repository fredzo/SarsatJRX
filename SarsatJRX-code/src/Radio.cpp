#include <Radio.h>
#include <SarsatJRXConf.h>

// Static members
Radio *Radio::radioInstance = nullptr;

const float frequecies[] = {406.0,406.025,406.037,406.040,406.049,430,433,0};


void Radio::radioInit()
{   // Init UART1
    radioSerial = &Serial1;
    // Serial begin is done here
    dra = new DRA818(radioSerial, SA818_UHF);
    // Now we can set pins
    if(!radioSerial->setPins(UART1_RX_PIN,UART1_TX_PIN))
    {
#ifdef SERIAL_OUT
        Serial.println("Could not set UART1 pins for SA818 communication");
#endif
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
    dra->handshake_async();
    dra->version_async();
    // Tests
    dra->group_async(DRA818_12K5, 406.0, 460.0, 0, 4, 0);
    dra->volume_async(8);
    dra->filters_async(true, false, true);
    dra->tail_async(true);
    setScanFrequencies(frequecies);
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
        retCode ? Serial.println("Group command success.") : Serial.println("Group command failed.");
    #endif
}

void Radio::handshakeCallback(int retCode)
{
    radioInstance->on = retCode;
    #ifdef SERIAL_OUT
        retCode ? Serial.println("Handshake command success.") : Serial.println("Handshake command failed.");
    #endif
}

void Radio::scanCallback(int retCode)
{
    radioInstance->scanFreqBusy = retCode;
    #ifdef SERIAL_OUT
        Serial.printf("Frequency %3.4f scan : %s\n",radioInstance->radioFrequency, radioInstance->scanFreqBusy ? "busy" : "no signal");
    #endif
    if(retCode) radioInstance->stopScan(); // Stop scan on busy frequencies
    if(radioInstance->scanOn)
    {
        radioInstance->scanNext();
    }
}

void Radio::volumeCallback(int retCode)
{
    #ifdef SERIAL_OUT
        retCode ? Serial.println("Volume command success.") : Serial.println("Volume command failed.");
    #endif
}

void Radio::filtersCallback(int retCode)
{
    #ifdef SERIAL_OUT
        retCode ? Serial.println("Filters command success.") : Serial.println("Filters command failed.");
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
        retCode ? Serial.println("Tail command success.") : Serial.println("Tail command failed.");
    #endif
}

void Radio::setScanFrequencies(const float* frequencies)
{
    scanFrequencies = frequencies;
    // Start scan will increment index before setting frequency
    currentScanFrequencyIndex = -1;
    if(frequecies)
    {
        radioFrequency = scanFrequencies[0];
    }
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
    parameters.freq_rx = radioFrequency;
    parameters.freq_tx = radioFrequency;
    dra->group_async(parameters);
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
    dra->group_async(parameters);
}

bool Radio::isScanFrequencyBusy()
{
    return scanFreqBusy;
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
    if(dra) dra->rssi_async();
    return power;
}

String Radio::getVersion()
{
    return version;
}

#include <Radio.h>
#include <SarsatJRXConf.h>

// Static members
Radio *Radio::radioInstance = nullptr;

const float frequecies[] = {406.0,406.025,406.037,406.040,406.049,430,433,0};


void Radio::radioInit()
{   // Init UART1
    radioSerial = &Serial1;
    // Seria begin is done here
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
    dra->scan_async(440.125);
    /*dra->scan(406.00);
    dra->scan(406.025);
    dra->scan(406.028);
    dra->scan(406.037);
    dra->scan(406.040);
    dra->scan(406.049);
    dra->scan(433.000);
    dra->scan(433.125);*/
    dra->read_group_async();
    dra->group_async(DRA818_25K, 406.025, 433.000, 25, 2, 38);
    dra->read_group_async();
    setScanFrequencies(frequecies);
    startScan();
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
        Serial.printf("Frequency %3.4f scan : %s\n",radioInstance->scanFrequency, radioInstance->scanFreqBusy ? "busy" : "no signal");
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
        scanFrequency = scanFrequencies[0];
    }
}

void Radio::scanNext()
{
    radioInstance->currentScanFrequencyIndex++;
    scanFrequency = scanFrequencies[currentScanFrequencyIndex];
    if(scanFrequency == 0)
    {
        currentScanFrequencyIndex = 0;
        scanFrequency = scanFrequencies[currentScanFrequencyIndex];
    }
    dra->scan_async(scanFrequency);
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
    parameters.freq_rx = scanFrequency;
    parameters.freq_tx = scanFrequency;
    dra->group_async(parameters);
}

float Radio::getCurrentScanFrequency()
{
    return scanFrequency;
}

bool Radio::isCurrentScanFrequencyBusy()
{
    return scanFreqBusy;
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

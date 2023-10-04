#include <Radio.h>
#include <SarsatJRXConf.h>

// Static members
Radio *Radio::radioInstance = nullptr;
int Radio::power = 0;

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
    dra->rssi_async_cb(Radio::rssiCallback);
    dra->handshake();
    dra->group(DRA818_12K5, 406.0, 460.0, 0, 4, 0);
    dra->volume(8);
    dra->filters(true, false, true);
    dra->tail(true);
    dra->scan(440.125);
    dra->scan(406.00);
    dra->scan(406.025);
    dra->scan(406.028);
    dra->scan(406.037);
    dra->scan(406.040);
    dra->scan(406.049);
    dra->scan(433.000);
    dra->scan(433.125);
    Serial.printf("Rssi = %d\n",dra->rssi());
    Serial.printf("Version = %s\n",dra->version().c_str());
    dra->read_group_async();
    dra->group_async(DRA818_25K, 406.025, 433.000, 25, 2, 38);
    dra->read_group_async();
    on = true;
}

void Radio::rssiCallback(int rssi)
{
    power = rssi;
}

void Radio::readGroupCallback(DRA818::Parameters parameters)
{
    Serial.println(parameters.toString().c_str());
}


void Radio::radioStop()
{
    on = false;
    free(dra);
    dra = NULL;
}

void Radio::handleTask()
{
    if(on)
    {
        dra->async_task();
    }
}

int Radio::getPower()
{   
    dra->rssi_async();
    return power;
}

String Radio::getVersion()
{
    return dra->version();
}

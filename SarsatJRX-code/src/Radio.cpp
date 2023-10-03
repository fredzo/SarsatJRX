#include <Radio.h>
#include <SarsatJRXConf.h>

Radio *Radio::radioInstance = nullptr;

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
    dra->handshake();
    dra->group(DRA818_12K5, 406.0, 460.0, 0, 4, 0);
    dra->volume(8);
    dra->filters(true, true, true);
    dra->rssi();
    dra->version();
}


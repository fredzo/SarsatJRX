#include <WifiManager.h>

#ifdef WIFI
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

// Web and Wifi
WebServer Server;
AutoConnect portal(Server);
AutoConnectConfig config;

// Wifi status
uint8_t wifiStatus = -1;

void rootPage()
{
  Server.send(200,"text/plain","--- SarsatJRX ---");
}

void wifiManagerStart()
{
  // Web and Wifi
  Server.on("/",rootPage);
  // Configure automatic reconnection and captive portal retention, then start
  // AutoConnect. In subsequent steps, it will use the portalStatus function to
  // detect the WiFi connection status in this configuration.
  config.autoReset = false;
  config.portalTimeout = 1;     // Don't block on AP mode
  config.beginTimeout = 2000;   // Only wait 2s at wifi begin not to block Sarsat JRX setartup
  config.autoReconnect = true;  // Automtic only if we have saved credentials
  config.reconnectInterval = 0; // Only reconnect on 1st connection failure
  config.retainPortal = true;
  config.hostName = "sarsatjrx";
  config.apid = "SarsatJRX";
  config.psk = "";              // No password in AP mode
  portal.config(config);
  portal.begin();
#ifdef SERIAL_OUT
  wifiManagerPrintStatus();
#endif
}

#ifdef SERIAL_OUT
void wifiManagerPrintStatus()
{
  uint8_t transition = portal.portalStatus();
  if (transition != wifiStatus) 
  {
    if (transition & AutoConnect::AC_CAPTIVEPORTAL)
    {
      Serial.println("Captive portal activated");
    }
    else if (transition & AutoConnect::AC_AUTORECONNECT)
    {
      Serial.println("Auto reconnection applied");
    }
    else if (!(transition & AutoConnect::AC_ESTABLISHED))
    {
      Serial.println("WiFi connection lost");
    }

    Serial.printf("Ip address : %s\n",WiFi.localIP().toString());
    WiFi.printDiag(Serial);

    wifiStatus = transition;
  }
}
#endif

void wifiManagerHandleClient()
{
    // Web and wifi
  portal.handleClient();
  //Server.handleClient();
#ifdef SERIAL_OUT
  wifiManagerPrintStatus();
#endif
}

#endif


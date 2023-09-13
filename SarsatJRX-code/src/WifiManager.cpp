#include <WifiManager.h>

#ifdef WIFI
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

// Web and Wifi
WebServer Server;
AutoConnect portal(Server);
AutoConnectConfig config;

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
  wifiUpdateStatus();
#endif
}

// Wifi status
int portalStatus = -1;
int wifiStatus = -1;
IPAddress ipAddr;
long lastStatusCheckTime = 0;
#define WIFI_STATUS_CHECK_PERIOD  500

bool wifiUpdateStatus()
{
  bool changed = false;
  long now = millis();
  if(now-lastStatusCheckTime >= WIFI_STATUS_CHECK_PERIOD)
  {
    int transition = portal.portalStatus();
    int wifi = WiFi.status();
    IPAddress locaIp = WiFi.localIP();
    if (transition != portalStatus) 
    {
      changed = true;
      portalStatus = transition;
  #ifdef SERIAL_OUT
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
    }
    if(wifi != wifiStatus)
    {
      changed = true;
      wifiStatus = wifi;
    }
    if(locaIp != ipAddr)
    {
      changed = true;
      ipAddr = locaIp;
    }
    if(changed)
    {
  #ifdef SERIAL_OUT
      Serial.println("Ip address : " + ipAddr.toString());
      WiFi.printDiag(Serial);
  #endif
    }
  }
  return changed;
}
#endif

bool wifiManagerHandleClient()
{
    // Web and wifi
  portal.handleClient();
  //Server.handleClient();
  return wifiUpdateStatus();
}

bool wifiManagerIsConnected()
{
  return (wifiStatus == WL_CONNECTED);
}

#endif

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
bool wifiStatusChanged = false;
int portalStatus = -1;
WifiStatus wifiStatus = WifiStatus::DISCONNECTED;
int wifiRssi = 0;
IPAddress ipAddr;
long lastStatusCheckTime = 0;

void rootPage()
{
  Server.send(200,"text/plain","--- SarsatJRX ---");
}

void onWifiEvent(WiFiEvent_t event) 
{
#ifdef SERIAL_OUT
    Serial.printf("[WiFi-event] event: %d\n", event);
#endif
    switch (event) 
    {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
#ifdef SERIAL_OUT
            Serial.println("Connected to access point");
#endif
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            // Update Wifi status when IP is assigned
            wifiStatus = WifiStatus::DISCONNECTED;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("Disconnected from WiFi access point");
#endif
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            // Update Wifi status when IP is assigned
            wifiStatus = WifiStatus::CONNECTED;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
#endif
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            // Update Wifi status when Access Point is activated (Portal mode)
            wifiStatus = WifiStatus::PORTAL;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("WiFi access point started");
#endif
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            // Update Wifi status when Access Point is desactivated (Portal mode)
            wifiStatus = WifiStatus::DISCONNECTED;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("WiFi access point  stopped");
#endif
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            // Update Wifi status when a connection to the protal is made
            wifiStatus = WifiStatus::PORTAL_CONNECTED;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("Client connected");
#endif
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            if(wifiStatus == WifiStatus::PORTAL_CONNECTED)
            { // Update Wifi status when disconnected form the protal
              wifiStatus = WifiStatus::PORTAL;
              wifiStatusChanged = true;
            }
#ifdef SERIAL_OUT
            Serial.println("Client disconnected");
#endif
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            // Update Wifi status when a connection to the protal is made
            wifiStatus = WifiStatus::PORTAL_CONNECTED;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("Assigned IP address to client");
#endif
            break;
        default: break;
    }
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
  config.beginTimeout = 3000;   // Only wait 3s at wifi begin not to block Sarsat JRX setartup
  config.autoReconnect = true;  // Automtic only if we have saved credentials
  config.reconnectInterval = 0; // Only reconnect on 1st connection failure
  config.retainPortal = true;
  config.hostName = "sarsatjrx";
  config.apid = "SarsatJRX";
  config.psk = "";              // No password in AP mode
  portal.config(config);
  WiFi.onEvent(onWifiEvent);
  portal.begin();
}

String wifiManagerGetStatusString()
{
  switch(wifiStatus)
  {
    case WifiStatus::CONNECTED : 
      return "Connected to Access Point";
    case WifiStatus::PORTAL :
      return "Portal started";
    case WifiStatus::PORTAL_CONNECTED:
      return "Portal started + client connected";
    case WifiStatus::DISCONNECTED:
    default:
      return "Disconnected";
  }
}

/**
 * Returns true if Wifi is connected
 */
WifiStatus wifiManagerGetStatus()
{
    return wifiStatus;  
}

/**
 * Returns true if Wifi is connected
 */
bool wifiManagerIsConnected()
{
    return (wifiStatus == WifiStatus::CONNECTED);
}

#define WIFI_STATUS_CHECK_PERIOD    2000 // Check every 2 seconds
#define WIFI_RSSI_CHANGE_THRESHOLD  10

bool wifiManagerHandleClient()
{
  // Web and wifi
  portal.handleClient();
  // Check for status change
  bool changed = wifiStatusChanged;
  long now = millis();
  if(wifiStatusChanged || (now-lastStatusCheckTime >= WIFI_STATUS_CHECK_PERIOD))
  {
    lastStatusCheckTime = now;
    IPAddress locaIp = WiFi.localIP();
    int rssi = WiFi.RSSI();
    if(locaIp != ipAddr)
    {
      changed = true;
      ipAddr = locaIp;
    }
    if(abs(rssi-wifiRssi)>WIFI_RSSI_CHANGE_THRESHOLD)
    {
      changed = true;
    }
    // Store new value anyway
    wifiRssi = rssi;
    if(changed)
    {
  #ifdef SERIAL_OUT
      WiFi.printDiag(Serial);
      Serial.println("Ip address : " + ipAddr.toString());
      Serial.println("Wifi status : " + wifiManagerGetStatusString());
      Serial.println("RSSI : "+ String(wifiRssi) + " dBm");
  #endif
    }
  }
  wifiStatusChanged = false;
  return changed;
}

#endif


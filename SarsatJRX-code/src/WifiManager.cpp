#include <WifiManager.h>

#ifdef WIFI
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Hardware.h>

#define FAVICON_FILE_PATH   "/sarsat-jrx.png"

// Web 
AsyncWebServer server(80);
// SSE
AsyncEventSource events("/sse");
// Wifi status
bool wifiStatusChanged = false;
bool ntpStatusChanged = false;
int portalStatus = -1;
WifiStatus wifiStatus = WifiStatus::OFF;
int wifiRssi = 0;
IPAddress ipAddr;
uint32_t lastStatusCheckTime = 0;
uint32_t lastConnectionAttempt = 0;
// Filesystem
bool filesystemMounted = false;
static FS* fileSystem = nullptr;

void rootPage(AsyncWebServerRequest *request)
{
  request->send(200,"text/plain","--- SarsatJRX ---");
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
            if(wifiStatus != WifiStatus::DISCONNECTED)
            {
              wifiStatus = WifiStatus::DISCONNECTED;
              wifiStatusChanged = true;
#ifdef SERIAL_OUT
              Serial.println("Disconnected from WiFi access point");
#endif
            }
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        {
            // Update Wifi status when IP is assigned
            wifiStatus = WifiStatus::CONNECTED;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
#endif
            break;
        }
        case ARDUINO_EVENT_WIFI_AP_START:
            // Update Wifi status when Access Point is activated (Portal mode)
            wifiStatus = WifiStatus::PORTAL;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("WiFi access point started");
#endif
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
        case ARDUINO_EVENT_WIFI_STA_STOP:
            // Update Wifi status when wifi is stopped
            wifiStatus = WifiStatus::OFF;
            wifiStatusChanged = true;
#ifdef SERIAL_OUT
            Serial.println("WiFi stopped");
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
  // Check SPIFSS
  Filesystems* filesystems = Hardware::getHardware()->getFilesystems();
  filesystemMounted = filesystems->isSpiFilesystemMounted();
  fileSystem = filesystems->getSpiFilesystem();
  // Web and Wifi
  wifiStatus = WifiStatus::DISCONNECTED;
  WiFi.onEvent(onWifiEvent);
  Settings* settings = Settings::getSettings();
  //Serial.printf("Starting wifi with ssid=%s and psk=%s\n",settings->getWifiSsid().c_str(),settings->getWifiPassPhrase().c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(settings->getWifiSsid().c_str(),settings->getWifiPassPhrase().c_str());
  /*if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
  }*/
  // Setup mDNS
  if (!MDNS.begin("sarsatjrx")) 
  {
    Serial.println("Error : could not start mDNS");
  }
  Serial.println("mDNS started : you can now access http://sarsatjrx.local/");  
  // Setup webserver
  server.on("/",rootPage);
  server.serveStatic("/favicon.ico", SPIFFS, FAVICON_FILE_PATH);
  // Setup SSE
  events.onConnect([](AsyncEventSourceClient *client)
  {
    if(client->lastId())
    {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    //send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!",NULL,millis(),1000);
  });
  events.onDisconnect([](AsyncEventSourceClient *client) 
  {
    Serial.printf("SSE Client disconnected! ID: %u\n", client->lastId());
  });

  server.addHandler(&events);  
  server.begin();
  wifiStatusChanged = true;
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
    case WifiStatus::OFF:
      return "Wifi is off";
    case WifiStatus::DISCONNECTED:
    default:
      return "Disconnected";
  }
}

static const char* wifiModes[] = { "OFF", "STA", "AP", "STA+AP" };

const char* wifiManagerGetMode()
{
  int mode = WiFi.getMode();
  return wifiModes[mode];
}

/**
 * Returns true if Wifi is connected
 */
WifiStatus wifiManagerGetStatus()
{
    return wifiStatus;  
}

void wifiManagerStop()
{
    server.end();
    MDNS.end();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
    wifiStatus = WifiStatus::OFF;
}

/**
 * Returns true if Wifi is connected
 */
bool wifiManagerIsConnected()
{
    return (wifiStatus == WifiStatus::CONNECTED);
}

void wifiManagerNtpSynched()
{
  ntpStatusChanged = true;
}


#define WIFI_STATUS_CHECK_PERIOD    2000 // Check every 2 seconds
#define WIFI_RSSI_CHANGE_THRESHOLD  10

bool wifiManagerHandleClient()
{
  // Check for status change
  bool changed = wifiStatusChanged;
  long now = millis();
  // TODO handle reconnection attempt

  if(wifiStatusChanged || (now-lastStatusCheckTime >= WIFI_STATUS_CHECK_PERIOD))
  {
    if ((wifiStatus == WifiStatus::DISCONNECTED) && ((now-lastConnectionAttempt) >= WIFI_RECONNECTION_TIEMOUT)) 
    { 
      lastConnectionAttempt = now;
      Serial.println("WiFi not connected. Reconnection attempt...");
      Settings* settings = Settings::getSettings();
      WiFi.disconnect();  // Force disconnect
      WiFi.begin(settings->getWifiSsid().c_str(),settings->getWifiPassPhrase().c_str());  // and reconnect
    }
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
      if(wifiStatus != WifiStatus::OFF) WiFi.printDiag(Serial); // WiFi.printDiag() crashes when wifi is off
      Serial.println("Ip address : " + ipAddr.toString());
      Serial.println("Wifi status : " + wifiManagerGetStatusString());
      Serial.println("RSSI : "+ String(wifiRssi) + " dBm");
  #endif
    }
    else if(ntpStatusChanged)
    {
      changed = true;
    }
  }
  wifiStatusChanged = false;
  return changed;
}

void wifiManagerSendTickerEvent(int countdown, String time)
{
  char buffer[32];
  snprintf(buffer,sizeof(buffer), "ticker;%d;%s",countdown,time); 
  events.send(NULL,buffer,millis());
}

void wifiManagerSendFrameEvent(bool valid, bool error)
{
  events.send(NULL,"frame",millis());
}

size_t wifiManagerClientCount()
{
  return events.count();
}



#endif


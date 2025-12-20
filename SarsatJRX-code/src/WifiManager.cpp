#include <WifiManager.h>

#ifdef WIFI
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Hardware.h>
#include <Util.h>
#include <SarsatJRXConf.h>

#define FAVICON_FILE_PATH   "/sarsat-jrx.png"

#define FRAME_SEPARATOR     "#\n"

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
// Current frame
Beacon* currentFrame = nullptr;

// For frames endpoint
extern Beacon* beacons[];
extern int beaconsWriteIndex;
extern int beaconsSize;
extern bool beaconsFull;
extern bool updateBuzzerLevel;
extern uint8_t buzzerLevel;
static int lastSentFrame = -1;

// For reset countdown endpoint
extern void stopCountdownAutoReload();

void rootPage(AsyncWebServerRequest *request)
{
  request->send(200,"text/plain","--- SarsatJRX ---");
}

void sseOptions(AsyncWebServerRequest *request)
{
  AsyncWebServerResponse *response = request->beginResponse(200);
  response->addHeader("Access-Control-Allow-Methods","POST, GET, OPTIONS");
  response->addHeader("Access-Control-Allow-Headers","cache-control, x-requested-with");
  request->send(response);
}

void frame(AsyncWebServerRequest *request)
{
  if(currentFrame)
  {
    request->send(200,"text/plain",currentFrame->toKvpString());
  }
  else
  {
    request->send(204,"text/plain");
  }
}

String serializeFrames(bool all)
{
    String result;
    if(beaconsSize == 0) return result; // Nothing to report
    int position;
    int size;
    if(all)
    {
      position = beaconsFull ? ((beaconsWriteIndex+1)%BEACON_LIST_MAX_SIZE) : 0;
      size = beaconsSize;
    }
    else
    { // No new frame since last send
      if(lastSentFrame == beaconsWriteIndex) return result;
      // Start after last sent frame
      position = lastSentFrame + 1;
      if(position >= BEACON_LIST_MAX_SIZE) position = 0;
      size = (position <= beaconsWriteIndex ? beaconsWriteIndex-position : BEACON_LIST_MAX_SIZE - (position - beaconsWriteIndex))+1;
      //Serial.printf("Partial frames position = %d writeIndex = %d, size = %d.\n",position,beaconsWriteIndex,size);
    }
    for(int i = 0 ; i < size ; i++)
    { // Iterate on each frame
      //Serial.printf("Serializing frame# = %d for position = %d, size = %d.\n",i,position,size);
      result += FRAME_SEPARATOR;
      result += beacons[position]->toKvpString();
      lastSentFrame = position;
      position++;
      if(position >= BEACON_LIST_MAX_SIZE) position = 0;
    }
    return result;
}

String serializeConfigEntry(String key, String value)
{
  return key + "=" + value + "\n";
}

String serializeConfig()
{   // Add all settings
    String config = Settings::getSettings()->toKvpString();
    Hardware* hardware = Hardware::getHardware();
    // Screen state
    config+= serializeConfigEntry("screenOn",boolToString(hardware->getDisplay()->isScreenOn()));
    // Wifi config
    // Mode (Station / Acess Point)
    config+= serializeConfigEntry("wifiMode",wifiManagerGetMode());
    // Status (Connected etc)
    config+= serializeConfigEntry("wifiStatus",wifiManagerGetStatusString());
    // Signal (RSSI)
    config+= serializeConfigEntry("wifiRssi",formatDbmValue(WiFi.RSSI()));
    // SSID
    config+= serializeConfigEntry("wifiCurrentSsid",WiFi.SSID());
    // IP @
    config+= serializeConfigEntry("wifiIP",WiFi.localIP().toString());
    // Gateway IP
    config+= serializeConfigEntry("wifiGatewayIP",WiFi.gatewayIP().toString());
    // DNS
    config+= serializeConfigEntry("wifiDNS1",WiFi.dnsIP(0).toString());
    config+= serializeConfigEntry("wifiDNS2",WiFi.dnsIP(1).toString());
    // Mac @
    config+= serializeConfigEntry("wifiMacAddress",WiFi.macAddress());
    // Mask
    config+= serializeConfigEntry("wifiSubnetMask",WiFi.subnetMask().toString());
    // Date
    Rtc* rtc = Rtc::getRtc();
    config+= serializeConfigEntry("rtcDate",rtc->getDateString() + " - " + rtc->getTimeString());
    config+= serializeConfigEntry("rtcNtpSync",boolToString(rtc->isNtpSynched()));
    // SD config
    Filesystems *filesystems = hardware->getFilesystems();
    config+= serializeConfigEntry("sdCardMounted",boolToString(filesystems->isSdFilesystemMounted()));
    // Total size
    config+= serializeConfigEntry("sdCardTotalBytes",String((uint32_t)filesystems->getSdTotalBytes()));
    // Used size
    config+= serializeConfigEntry("sdCardUsedBytes",String((uint32_t)filesystems->getSdUsedBytes()));
    // System info
    config+= serializeConfigEntry("firmwareVersion",SARSAT_JRX_VERSION);
    config+= serializeConfigEntry("sketchInfo",formatSketchInformation(ESP.getSketchSize(),ESP.getSketchMD5()));
    config+= serializeConfigEntry("chipModel",ESP.getChipModel());
    config+= serializeConfigEntry("chipCores",String(ESP.getChipCores()));
    config+= serializeConfigEntry("chipFrequency",String(ESP.getCpuFreqMHz()));
    // Ram
    config+= serializeConfigEntry("ramSize",String(ESP.getHeapSize()));
    config+= serializeConfigEntry("ramFree",String(ESP.getFreeHeap()));
    // PS Ram
    config+= serializeConfigEntry("psRamSize",String(ESP.getPsramSize()));
    config+= serializeConfigEntry("psRamFree",String(ESP.getFreePsram()));
    // Flash
    config+= serializeConfigEntry("flashSize",String(ESP.getFlashChipSize()));
    config+= serializeConfigEntry("flashFreq",String(ESP.getFlashChipSpeed()));
    // Vbat
    Power* power = hardware->getPower();
    // Vbat
    config+= serializeConfigEntry("powerVcc",power->getVccStringValue());
    // Power state
    config+= serializeConfigEntry("powerState",power->getPowerStateString());
    config+= serializeConfigEntry("powerBatteryPercentage",String(power->getBatteryPercentage()));
    // Uptime
    config+= serializeConfigEntry("upTime",String(rtc->getUptimeString()));

    return config;
}

void frames(AsyncWebServerRequest *request)
{
  if(beaconsSize > 0)
  { // Start at first frame in list
    request->send(200,"text/plain",serializeFrames(true));
  }
  else
  {
    request->send(204,"text/plain");
  }
}

void resetCountdown(AsyncWebServerRequest *request)
{
    // Audio feedback for button press
    SoundManager::getSoundManager()->playFilteredFrameSound();
    Rtc* rtc = Rtc::getRtc();
    if(rtc->getCountDown() >= 0)
    {   // Stop countdown from auto reloading
        stopCountdownAutoReload();
    }
    else
    {   // Restart countdown
        rtc->startCountDown();
    }
    request->send(200);
}

void postConfig(AsyncWebServerRequest *request)
{
    size_t paramsNumber = request->params();
    Settings* settings = Settings::getSettings();
    Hardware* hardware = Hardware::getHardware();
    Display* display = hardware->getDisplay();
    for(size_t i = 0 ; i < paramsNumber ; i++)
    {
      const AsyncWebParameter* param = request->getParam(i);
      String name = param->name();
      String value = param->value();
      if(name == "screenOn")
      {
        bool screenOn = (value == "true");
        display->setScreenOn(screenOn);
        // No settings for this
        continue;
      }
      else if(name == "displayReverse")
      {
        bool reverse = (value == "true");
        display->setScreenReverse(reverse);
      }
      else if(name == "buzzerLevel")
      {
        buzzerLevel = stringToUChar(value);
        updateBuzzerLevel = true;
      }
      settings->setSettingValue(name,value,false);
      if(name.startsWith("wifiSsid") || name.startsWith("wifiPassPhrase") || name.startsWith("timeZone"))
      { // Save settings right now for Strings
        settings->save();
      }
      display->updateSettings();
    }
    request->send(200);
}

#define SCREENSHOT_BUFFER_SIZE DISPLAY_WIDTH*DISPLAY_HEIGHT*3+1024
static uint8_t* screenBuffer = nullptr;
static uint8_t* imageBuffer = nullptr;

void prepareBuffers()
{
    if(!screenBuffer) screenBuffer = (uint8_t*) ps_malloc(DISPLAY_WIDTH*DISPLAY_HEIGHT*3);
    if(!imageBuffer) imageBuffer = (uint8_t*) ps_malloc(SCREENSHOT_BUFFER_SIZE);
    
    if (!screenBuffer || !imageBuffer) 
    {
      Serial.println("Allocating buffers failed !");
    }
}

void takeScreenshot(AsyncWebServerRequest *request)
{
  size_t screenshotSize;
  if(!screenBuffer || !imageBuffer)
  {
      Serial.println("No memory !");
      request->send(500,"text/plain","No memory !");
      return;
  }
  Display* display = Hardware::getHardware()->getDisplay();
  display->screenshot(screenBuffer);
  screenshotSize = screenshotToBmp(screenBuffer,imageBuffer,DISPLAY_WIDTH,DISPLAY_HEIGHT);
  AsyncResponseStream *response = request->beginResponseStream("image/bmp");
  response->addHeader("Cache-Control", "no-store");
  response->addHeader("Content-Disposition", "inline; filename=\"screenshot.bmp\"");
  response->write(imageBuffer, screenshotSize);
  request->send(response);
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

void wifiManagerConnect()
{
  Settings* settings = Settings::getSettings();
  String ssid;
  int n = WiFi.scanNetworks();
  if (n == 0) 
  {
#ifdef SERIAL_OUT
    Serial.println("No wifi network available.");
#endif
    return;
  }
#ifdef SERIAL_OUT
  Serial.printf("Found %d wifi network(s)\n", n);
#endif

  // Iterate on each registered network and check for it's availability
  for (int i = 0; i < settings->getWifiNetworkNumber(); i++) 
  {
    ssid = settings->getWifiSsid(i);
    for (int j = 0; j < n; j++) 
    {
      if (strcmp(ssid.c_str(), WiFi.SSID(j).c_str()) == 0) 
      { // Network available
#ifdef SERIAL_OUT
          Serial.printf("Connection attempt to %s...\n", ssid.c_str());
#endif          
        WiFi.begin(ssid.c_str(), settings->getWifiPassPhrase(i).c_str());
        /*if (WiFi.waitForConnectResult() != WL_CONNECTED) {
          Serial.printf("WiFi Failed!\n");
        }*/
        //Serial.printf("Starting wifi with ssid=%s and psk=%s\n",settings->getWifiSsid().c_str(),settings->getWifiPassPhrase().c_str());
        return;
      }
    }
  }
#ifdef SERIAL_OUT
  Serial.println("No registered wifi network available.");  
#endif          
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
  WiFi.mode(WIFI_STA);
  wifiManagerConnect();
  // Setup mDNS
  if (!MDNS.begin("sarsatjrx")) 
  {
    Serial.println("Error : could not start mDNS");
  }
  // Add service to mDNS
  MDNS.addService("_http", "_tcp", 80);
  Serial.println("mDNS started : you can now access http://sarsatjrx.local/");  
  // Prepare buffers
  prepareBuffers();
  // Add CORS headers
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  // Setup webserver
  server.on("/",rootPage);
  server.on("/frame",frame);
  server.on("/frames",frames);
  server.on("/resetcd",resetCountdown);
  server.on("/config",HTTP_POST,postConfig);
  server.on("/sse",HTTP_OPTIONS,sseOptions);
  server.serveStatic("/favicon.ico", SPIFFS, FAVICON_FILE_PATH, "public, max-age=31536000, immutable");
  server.on("/screenshot",HTTP_GET,takeScreenshot);

  // Setup SSE
  events.onConnect([](AsyncEventSourceClient *client)
  {
    if(client->lastId())
    {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    if(wifiStatus == WifiStatus::CONNECTED)
    {
      wifiStatus = WifiStatus::LINKED;
      wifiStatusChanged = true;
    }
    // Send config
    wifiManagerSendConfigEvent();
    // Send frames if any
    String frames = serializeFrames(false);
    if(!frames.isEmpty())
    {
      String message = "frames\n" + frames;
      events.send(message.c_str());
    }
    // Send connected event
    events.send("connected");
  });
  events.onDisconnect([](AsyncEventSourceClient *client) 
  {
    if(wifiStatus == WifiStatus::LINKED)
    {
      wifiStatus = WifiStatus::CONNECTED;
      wifiStatusChanged = true;
    }
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
    case WifiStatus::LINKED : 
      return "Application linked to decoder";
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
    return (wifiStatus == WifiStatus::CONNECTED || wifiStatus == WifiStatus::LINKED);
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
      // Force disconnect
      WiFi.disconnect();
      // and reconnect
      wifiManagerConnect();
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

void wifiManagerSendTickerEvent(int countdown,bool sdMounted, bool discriOn, int batPercentage, String time)
{
  char buffer[32];
  snprintf(buffer,sizeof(buffer), "tick;%d;%d;%d;%d;%s",countdown,(sdMounted ? 1 : 0),(discriOn ? 1 : 0),batPercentage,time); 
  events.send(buffer);
}

void wifiManagerSendFrameEvent(Beacon* beacon, int frameIndex, bool valid, bool filtered, bool error)
{
  if(beacon) currentFrame = beacon;
  if(events.count()>0)
  {
    char buffer[16];
    snprintf(buffer,sizeof(buffer), "frame;%d;%d;%d",valid,filtered,error);
    if(beacon)
    {
      String message = String(buffer) + "\n" + beacon->toKvpString();
      events.send(message.c_str());
      lastSentFrame = frameIndex;
    }
    else
    {
      events.send(buffer);
    }
  }
}

void wifiManagerSendConfigEvent()
{ // Send config
  String config = serializeConfig();
  String message = "config\n" + config;
  events.send(message.c_str());
}

size_t wifiManagerClientCount()
{
  return events.count();
}



#endif


/*******************************
   SarsatJRX
   406 Mhz Beacon Frame decoder
   Hardware : TTGO LilyPi
********************************/

#include <Arduino.h>

#include <SarsatJRXConf.h>
#include <Hardware.h>
#include <Location.h>
#include <Country.h>
#include <Beacon.h>
#include <Util.h>
#include <Samples.h>
#include <Decoder.h>
#include <ui/Ui.h>
#include <WifiManager.h>

// Header LEDS
bool ledSig1State = false;
bool ledSig2State = false;
bool ledInFrameState = false;
bool ledFrameReceivedState = false;

// Beacon list
#define BEACON_LIST_MAX_SIZE  20

Beacon* beacons[BEACON_LIST_MAX_SIZE];
int beaconsWriteIndex = -1;
int beaconsReadIndex = -1;
int beaconsSize = 0;
bool beaconsFull = false;

Hardware* hardware = nullptr; 
Display *display = nullptr;

void readBeacon()
{
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  if(beaconsFull)
  {
    beaconsWriteIndex++;
  }
  else
  { // Move to the end of the list
    beaconsWriteIndex = beaconsSize;
    if(beaconsSize < BEACON_LIST_MAX_SIZE)
    {
      beaconsSize++;
    }
    else
    {
      beaconsFull = true;
    }
  }
  if(beaconsWriteIndex >= BEACON_LIST_MAX_SIZE)
  { // Circle back to zero
    beaconsWriteIndex = 0;
  }
  // Delete previously stored beacon to prevent memory leak
  delete beacons[beaconsWriteIndex];
  Beacon* beacon;
  if(isFrameFromDisk())
  {
    beacon = new Beacon(getFrame(),getDiskFrameDate());
  }
  else
  {
    beacon = new Beacon(getFrame());
  }
  // Add beacon to the list
  beacons[beaconsWriteIndex] = beacon;
  // Move to last received
  beaconsReadIndex = beaconsWriteIndex;

#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
}

// Leds management (header + physical led)
#define LED_BLINK_TIME 1000 // 1 s
unsigned long ledFrameReceivedStartBlinkTime;
unsigned long ledSig1StartBlinkTime;
unsigned long ledSig2StartBlinkTime;
bool ledSig1On = false;
bool ledSig2On = false;
bool ledInFrameOn = false;
bool ledFrameReceivedOn = false;
bool lastInputState = false;

void updateLeds()
{
  if(ledFrameReceivedOn && ((millis() - ledFrameReceivedStartBlinkTime)>LED_BLINK_TIME))
  { // Led has been on for more than 100 ms => switch it off
    digitalWrite(NOTIFICATION_PIN, LOW); 
    ledFrameReceivedOn = false;
  }
}

void frameReceivedLedBlink()
{ // Switch led on and record time
  digitalWrite(NOTIFICATION_PIN, HIGH);
  ledFrameReceivedOn = true;
  ledFrameReceivedStartBlinkTime = millis();
}

// Store last displayed power value
unsigned long lastPowerDisplayTime = 0;
float powerValue = -1;
#define POWER_DISPLAY_PERIOD 500

void updatePowerValueHeader()
{  // Power header
  uiSetPower(hardware->getVccStringValue().c_str());
}

void updateTimeHeader()
{  // Time header
  uiSetTime(hardware->getRtc()->getTimeString().c_str());
}

void updateLedHeader(bool force)
{ // Led header
  bool newInputState = getInputState();
  bool drawledSig1 = force;
  bool drawledSig2 = force;
  bool drawledInFrame = force;
  bool drawledFrameReceived = force;
  unsigned long now = millis();
  if(newInputState != lastInputState)
  { // Flip headerLedSig1 and headerLedSig2 according to decoder input stage state
    lastInputState = newInputState;
    ledSig1State = newInputState;
    ledSig2State = !newInputState;
    drawledSig1 = true;
    drawledSig2 = true;
    if(ledSig1State) ledSig1StartBlinkTime = now;
    if(ledSig2State) ledSig2StartBlinkTime = now;
  }
  else
  { // Check if we have to turn decoder input leds off
    if(ledSig1State && ((now - ledSig1StartBlinkTime) > LED_BLINK_TIME))
    {
      ledSig1State = false;
      drawledSig1 = true;
    }
    if(ledSig2State && ((now - ledSig2StartBlinkTime) > LED_BLINK_TIME))
    {
      ledSig2State = false;
      drawledSig2 = true;
    }
  }
  if(ledInFrameState != isFrameStarted())
  { // Check if frame started state changed
    ledInFrameState = isFrameStarted();
    drawledInFrame = true;
  }
  if(ledFrameReceivedState != ledFrameReceivedOn)
  { // Check if frame received state changed
    ledFrameReceivedState = ledFrameReceivedOn;
    drawledFrameReceived = true;
  }
  if(drawledSig1) uiSetLedSig1State(ledSig1State);
  if(drawledSig2) uiSetLedSig2State(ledSig2State);
  if(drawledInFrame) uiSetLedInFrameState(ledInFrameState);
  if(drawledFrameReceived) uiSetLedFrameReceivedState(ledFrameReceivedState);
}

void updateHeader()
{ // Check for time update
  if(hardware->getRtc()->hasChanged())
  {
    updateTimeHeader();
  }
  // Check for power value update
  unsigned long now = millis();
  if(now - lastPowerDisplayTime > POWER_DISPLAY_PERIOD)
  {
    lastPowerDisplayTime = now;
    float newValue = hardware->getVccValue();
    if(abs(newValue - powerValue) > 0.01)
    {
      powerValue = newValue;
      updatePowerValueHeader();
    }
  }
  updateLedHeader(false);
}

void initHeader()
{
  // Time
  updateTimeHeader();
  // Draw leds
  updateLedHeader(true);
}

// Footer management
unsigned long lastFooterUpdateTime;
bool footerShowingFrameReceived = false;
bool footerShowingSpinner = false;
int spinnerPosition = 0;
#define FOOTER_FRAME_RECEIVED_TIME 2000
unsigned long lastPowerUpdateTime;
#define FOOTER_POWER_UPDATE_TIME 100
float lastFreq = -1;
bool lastScanOn = false;
bool lastRadioOn = false;

void updateFooter(bool frameReceived)
{  
  unsigned long now = millis();
  if(footerShowingFrameReceived)
  {
    if((now - lastFooterUpdateTime) > FOOTER_FRAME_RECEIVED_TIME)
    {
      lastFooterUpdateTime = now;
      uiShowFrameReceived(false);
      footerShowingFrameReceived = false;
    }
  }
  else
  {
    if(frameReceived)
    {
      lastFooterUpdateTime = now;
      uiShowFrameReceived(true);
      footerShowingFrameReceived = true;
    }
  }
  // Radio
  Radio* radio = hardware->getRadio();
  bool radioOn = radio->isRadioOn();
  if(radioOn != lastRadioOn)
  {
    uiSetRadioStatus(radioOn);
    lastRadioOn = radioOn;
  }
  if(radioOn)
  {
    if((now - lastPowerUpdateTime) > POWER_DISPLAY_PERIOD)
    {
      lastPowerUpdateTime = now;
      uiSetPower(radio->getPower());
    }
    float freq = radio->getFrequency();
    bool scanOn = radio->isScanOn();
    if((freq != lastFreq) || (scanOn != lastScanOn))
    {
      uiSetFreq(freq,scanOn);
      lastFreq = freq;
      lastScanOn = scanOn;
    }
  }
}

void updateDisplay()
{
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  // Rotating index based on beacon list max size and position of last read frame
  int displayIndex = ((beaconsSize-1 + beaconsReadIndex - beaconsWriteIndex) % BEACON_LIST_MAX_SIZE)+1;
  uiSetBeacon(beacons[beaconsReadIndex],displayIndex,beaconsSize);
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif

}

void readNextSampleFrame()
{ // Read the frame content
  readNextSample(getFrame());
  // Tell the state machine that we have a complete frame
  setFrameComplete(true);
}

void toggleScan()
{
  hardware->getRadio()->toggleScan();
}

bool readBeaconFromFile(const char* fileName)
{ // Read the frame content
  if(hardware->getFilesystems()->loadBeacon(fileName,getFrame()))
  { // Tell the state machine that we have a complete frame
    setFrameComplete(true);
    setFrameFromDisk(parseBeaconFileName(fileName));
    return true;
  }
  else
  {
    return false;
  }
}

void setup()
{
  pinMode(RECEIVER_PIN, INPUT);          // Detection stage inpit
  pinMode(NOTIFICATION_PIN, OUTPUT);     // Notification led output
  // pinMode(16, OUTPUT);                  // Buzzer output
  
  Serial.begin(115200);
  //attachInterrupt(digitalPinToInterrupt(receiverPin), analyze, CHANGE);  // interruption sur Rise et Fall

  hardware = Hardware::getHardware();
  hardware->init();
  display = hardware->getDisplay();

  // Build the UI
  createUi();
  initHeader();
  uiSetRadioStatus(hardware->getRadio()->isRadioOn());

#ifdef WIFI
  if(hardware->getSettings()->getWifiState())
  {
    wifiManagerStart();
    uiSetWifiStatus(wifiManagerGetStatus());
  }
#endif

  uiSetSdCardStatus(hardware->getFilesystems()->isSdFilesystemMounted());
  
  //readNextSampleFrame();
#ifdef SERIAL_OUT 
  Serial.println("### Boot complete !");
#endif
}

void previousFrame()
{
    beaconsReadIndex--;
    if(beaconsReadIndex<0)
    {
      beaconsReadIndex = beaconsSize-1;
    }
    updateDisplay();
}

void nextFrame()
{
    beaconsReadIndex++;
    if(beaconsReadIndex>=beaconsSize)
    {
      beaconsReadIndex = 0;
    }
    updateDisplay();
}

void loop()
{
  // If frameParseState > 0, we are reading an frame, if more thant 500ms elapsed (a frame should be 144x2.5ms = 360 ms max)
  bool frameTimeout = (isFrameStarted() && ((millis()-getFrameStartTime()) > 500 ));
  if (isFrameComplete() || frameTimeout)
  {
    volatile byte* frame = getFrame();
    // Debug purpose 
 #ifdef SERIAL_OUT 
   if(frameTimeout)
    {
      Serial.println("Frame timeout !");
    }
    for ( byte i = 0; i < Beacon::SIZE; i++) // RAW data
    {
      if (frame[i] < 16)
      Serial.print('0');
      Serial.print (frame[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  #endif
  #ifdef DEBUG_DECODE
    int* events = getEvents();
    int eventCount = getEventCount();
    for ( int i = 0; i < eventCount; i++)
    {
      Serial.println(events[i],DEC);
    }
    Serial.println("");
    bool* bits = getBits();
    int bitCount = getBitCount();
    for (int i = 0; i < bitCount; i++)
    {
      Serial.print(bits[i],DEC);
    }
    Serial.println("");
  #endif    

    if (((frame[1] == 0xFE) && (frame[2] == 0xD0)) || ((frame[1] == 0xFE) && (frame[2] == 0x2F)))// 0XFE/0x2F for normal mode, 0xFE/0xD0  for autotest
    { // First blick leds
      frameReceivedLedBlink();
      updateLeds();
      updateHeader();
      display->handleTimer();
      // Then read beacon and update beacon display
      readBeacon();
      updateDisplay();
      display->handleTimer();
      if(!isFrameFromDisk())
      { // Finally save beacon to sd card
        bool success = hardware->getFilesystems()->saveBeacon(beacons[beaconsReadIndex]);
        if(!success)
        { // Update SD Card indicator
          uiSetSdCardStatus(hardware->getFilesystems()->isSdFilesystemMounted());
        }
      }
    } 
    // Reset frame decoding
    updateFooter(true);
    resetFrameReading();
  }
  else
  {
    updateFooter(false);
  }
  updateLeds();
  updateHeader();
  display->handleTimer();
  // Radio task
  hardware->getRadio()->handleTask();

    // Web and wifi
#ifdef WIFI
    if(wifiManagerHandleClient())
    {
      uiSetWifiStatus(wifiManagerGetStatus());
    }
#endif
  //delay(5);
}


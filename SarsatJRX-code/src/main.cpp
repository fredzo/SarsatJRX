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
// Power
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"


// Interupt pin : use digital pin 18
const int receiverPin = 21;
// Notification led : use digital pin 19
const int notificationPin = 22;

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
  Beacon* beacon = new Beacon(getFrame());
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
    digitalWrite(notificationPin, LOW); 
    ledFrameReceivedOn = false;
  }
}

void frameReceivedLedBlink()
{ // Switch led on and record time
  digitalWrite(notificationPin, HIGH);
  ledFrameReceivedOn = true;
  ledFrameReceivedStartBlinkTime = millis();
}

// Store last displayed power value
unsigned long lastPowerDisplayTime = 0;
float powerValue = -1;
#define POWER_DISPLAY_PERIOD 2000

void updatePowerValueHeader()
{  // Power header
  char powerString[8];
  hardware->getVccStringValue(powerString);
  char buffer[8];
  sprintf(buffer,HEADER_POWER_TEMPLATE,powerString);
  uiSetPower(buffer);
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

void setup()
{
  pinMode(receiverPin, INPUT);          // Detection stage inpit
  pinMode(notificationPin, OUTPUT);     // Notification led output
  // pinMode(16, OUTPUT);                  // Buzzer output
  
  Serial.begin(115200);
  //attachInterrupt(digitalPinToInterrupt(receiverPin), analyze, CHANGE);  // interruption sur Rise et Fall

  hardware = Hardware::getHardware();
  hardware->init();
  display = hardware->getDisplay();

  // Build the UI
  createUi();
  initHeader();

#ifdef WIFI
  wifiManagerStart();
  uiSetWifiStatus(wifiManagerGetStatus());
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
    byte* frame = getFrame();
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
      // Finally save beacon to sd card
      hardware->getFilesystems()->saveBeacon(beacons[beaconsReadIndex]);
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

    // Web and wifi
#ifdef WIFI
    if(wifiManagerHandleClient())
    {
      uiSetWifiStatus(wifiManagerGetStatus());
    }
#endif
  //delay(5);
}


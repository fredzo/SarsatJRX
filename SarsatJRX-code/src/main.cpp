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
#include <WifiManager.h>
#include <BluetoothManager.h>

// Nedded if stack size of Arduino loop is too small
// SET_LOOP_TASK_STACK_SIZE(16*1024)

// Disri jack state
bool discriJackPlugged = false;

// Beacon list
#define BEACON_LIST_MAX_SIZE  16

Beacon* beacons[BEACON_LIST_MAX_SIZE];
int beaconsWriteIndex = -1;
int beaconsReadIndex = -1;
int beaconsSize = 0;
bool beaconsFull = false;

Hardware* hardware = nullptr; 
Display *display = nullptr;

typedef enum { BEACON_FILTER_NONE, BEACON_FILTER_INVALID, BEACON_FILTER_ORBITO } BeaconFilter;

BeaconFilter readBeacon()
{
  BeaconFilter filter = BEACON_FILTER_NONE;
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  // Start by reading beacon
  Beacon* beacon;
  if(isFrameFromDisk())
  {
    beacon = new Beacon(getFrame(),getDiskFrameDate());
  }
  else
  {
    beacon = new Beacon(getFrame());
  }
  // See if we need to filter this beacon
  Settings* settings = hardware->getSettings();
  if((!beacon->isFrameValid())&&settings->getFilterInvalid())
  {
    filter = BEACON_FILTER_INVALID;
  }
  else if(beacon->isOrbito()&&settings->getFilterOrbito())
  {
    filter = BEACON_FILTER_ORBITO;
  }

  if(filter == BEACON_FILTER_NONE)
  { // No filtering => store the beacon in list
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
    if(beacons[beaconsWriteIndex]) 
    {
        delete beacons[beaconsWriteIndex];
        beacons[beaconsWriteIndex] = nullptr;
    }
    // Add beacon to the list
    beacons[beaconsWriteIndex] = beacon;
    // Move to last received
    beaconsReadIndex = beaconsWriteIndex;
  }
  else
  { // Filter beacon => delete it to free memory
    delete beacon;
    beacon = nullptr;
  }

#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  return filter;
}

// Header LEDS
bool ledSig1State = false;
bool ledSig2State = false;
bool ledInFrameState = false;
bool ledFrameErrorState = false;
bool ledFrameReceivedState = false;

// Leds management (header + physical led)
#define LED_BLINK_TIME 1000 // 1 s
#define LED_BLINK_SHORT_TIME 100 // 100 ms
unsigned long ledsStartBlinkTime;
unsigned long ledSig1StartBlinkTime;
unsigned long ledSig2StartBlinkTime;
bool ledSig1On = false;
bool ledSig2On = false;
bool ledFrameErrorOn = false;
bool ledFrameReceivedOn = false;
bool lastInputState = false;
bool countDownModeOn = false;

void updateLeds()
{
  if((ledFrameReceivedOn || ledFrameErrorOn) && ((millis() - ledsStartBlinkTime)> (countDownModeOn ? LED_BLINK_SHORT_TIME : LED_BLINK_TIME)))
  { // Leds have been on long enough (long blink or short blink accordig to countdown mode beeing on or not)
    digitalWrite(NOTIFICATION_PIN, LOW); 
    digitalWrite(ERROR_PIN, LOW); 
    ledFrameReceivedOn = false;
    ledFrameErrorOn = false;
    countDownModeOn = false;
  }
}

void frameReceivedLedBlink()
{ // Switch led on and record time
  digitalWrite(NOTIFICATION_PIN, HIGH);
  ledFrameReceivedOn = true;
  ledsStartBlinkTime = millis();
  countDownModeOn = false;
}

void invalidFrameReceivedLedBlink()
{ // Switch led on and record time
  digitalWrite(ERROR_PIN, HIGH);
  ledFrameErrorOn = true;
  ledsStartBlinkTime = millis();
  countDownModeOn = false;
}

// Store last displayed power value
unsigned long lastPowerDisplayTime = 0;

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
  if((ledInFrameState != isFrameStarted()) || (ledFrameErrorState != ledFrameErrorOn))
  { // Check if frame started state changed
    ledInFrameState = isFrameStarted() || ledFrameErrorOn;
    ledFrameErrorState = ledFrameErrorOn;
    drawledInFrame = true;
  }
  if(ledFrameReceivedState != ledFrameReceivedOn)
  { // Check if frame received state changed
    ledFrameReceivedState = ledFrameReceivedOn;
    drawledFrameReceived = true;
  }
  if(drawledSig1) display->updateLedSig1(ledSig1State);
  if(drawledSig2) display->updateLedSig2(ledSig2State);
  if(drawledInFrame) display->updateLedInFrame(ledInFrameState,ledFrameErrorState);
  if(drawledFrameReceived) display->updateLedFrameReceived(ledFrameReceivedState);
}

void updateHeader()
{ // Check for time update
  Rtc* rtc = hardware->getRtc();
  if(rtc->hasChanged())
  {
    display->updateTime();
  }
  // Check for power value update
  if(hardware->getPower()->hasChanged())
  {
    display->updatePower();
  }
  Audio* audio = hardware->getAudio();
  if(audio->discriHasChanged())
  {
    discriJackPlugged = audio->isDiscriInput();
    display->updateDiscri();
  }
  updateLedHeader(false);
}

void initHeader()
{
  // Time
  display->updateTime();
  // Draw leds
  updateLedHeader(true);
}

// Footer management
unsigned long lastFooterUpdateTime;
bool footerShowingFrameReceived = false;
bool footerShowingSpinner = false;
int spinnerPosition = 0;
#define FOOTER_FRAME_RECEIVED_TIME 2000
unsigned long lastAudioPowerUpdateTime;
#define FOOTER_POWER_UPDATE_TIME 100

void updateFooter(bool frameReceived)
{  
  unsigned long now = millis();
  if(footerShowingFrameReceived)
  {
    if((now - lastFooterUpdateTime) > FOOTER_FRAME_RECEIVED_TIME)
    {
      lastFooterUpdateTime = now;
      display->updateFrameReceived(false);
      footerShowingFrameReceived = false;
    }
  }
  else
  {
    if(frameReceived)
    {
      lastFooterUpdateTime = now;
      display->updateFrameReceived(true);
      footerShowingFrameReceived = true;
    }
  }
  // Audio
  if(!discriJackPlugged && (now - lastAudioPowerUpdateTime) > AUDIO_POWER_DISPLAY_PERIOD)
  { // Only update audio metter if dscri jack is not plugged
    lastAudioPowerUpdateTime = now;
    display->updateAudioPower();
  }
}

void updateDisplay()
{
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  // Rotating index based on beacon list max size and position of last read frame
  int displayIndex = ((beaconsSize-1 + beaconsReadIndex - beaconsWriteIndex) % BEACON_LIST_MAX_SIZE)+1;
  display->updateBeacon(beacons[beaconsReadIndex],displayIndex,beaconsSize);
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

void stopCountdownAutoReload()
{ // Skip directly to -1 to prevent auto-reload
  Rtc::getRtc()->setCountDown(-1);
}

void setup()
{ // Init beacon list
  for(int i=0; i<BEACON_LIST_MAX_SIZE; i++) beacons[i] = nullptr;

  // For heap corruption debugging
  //heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

  pinMode(RECEIVER_PIN, INPUT);          // Detection stage input
  pinMode(NOTIFICATION_PIN, OUTPUT);     // Notification led output
  pinMode(ERROR_PIN, OUTPUT);            // Error led output
  pinMode(BUZZER_PIN, OUTPUT);           // Buzzer output


  
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), analyze, CHANGE);  // interruption sur Rise et Fall

  hardware = Hardware::getHardware();
  hardware->init();
  display = hardware->getDisplay();

#ifdef WIFI
  if(hardware->getSettings()->getWifiState())
  {
    wifiManagerStart();
    display->updateWifi();
  }
#endif

  display->updateSdCard();
  display->updateDiscri();

  Settings* settings = hardware->getSettings();

  display->setReverse(settings->getDisplayReverse());

  // UI is now ready : start display task
  display->startDisplayTask();

  // Init buzzer level
  hardware->getSoundManager()->setVolume(settings->getBuzzerLevel());

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

void blinkNotifLed()
{
  if(!ledFrameReceivedOn)
  { // Blink leds if not already showing received frame
    digitalWrite(NOTIFICATION_PIN, HIGH);
    ledFrameReceivedOn = true;
    ledsStartBlinkTime = millis();
    countDownModeOn = true;
  }
}

void blinkErrorLed()
{
  if(!ledFrameReceivedOn)
  { // Blink leds if not already showing received frame
    digitalWrite(ERROR_PIN, HIGH);
    ledFrameErrorOn = true;
    ledsStartBlinkTime = millis();
    countDownModeOn = true;
  }
}

void loop()
{
  // Audio task
  hardware->getAudio()->handleTask();
  // Power task
  hardware->getPower()->handleTask();
  // Get rtc
  Rtc* rtc = hardware->getRtc();
  Settings* settings = hardware->getSettings();
  // If frameParseState > 0, we are reading an frame, if more than 500ms elapsed (a frame should be 144x2.5ms = 360 ms max)
  bool frameTimeout = (isFrameStarted() && ((millis()-getFrameStartTime()) > 500 ));
  if (isFrameComplete() || frameTimeout)
  { // Get frame content
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

    if(((frame[1] == 0xFE) && (frame[2] == 0xD0)) || ((frame[1] == 0xFE) && (frame[2] == 0x2F)))// 0XFE/0x2F for normal mode, 0xFE/0xD0  for autotest
    { // Blink blue led
      frameReceivedLedBlink();
      // Then read beacon and update beacon display
      BeaconFilter filter = readBeacon();
      if(filter == BEACON_FILTER_NONE)
      { // Start countdown for next frame only if not filtered
        rtc->startCountDown();            
        // Play sound and update error led and display
        bool error = !(beacons[beaconsReadIndex]->isFrameValid());
        if(error)
        { // Error led
          invalidFrameReceivedLedBlink();
          Serial.println("Frame error !");
        }
        if(settings->getFrameSound())
        {
          hardware->getSoundManager()->playFrameSound(error);
        }
        updateDisplay();
        if(!isFrameFromDisk())
        { // Finally save beacon to sd card
          bool success = hardware->getFilesystems()->saveBeacon(beacons[beaconsReadIndex]);
          if(!success)
          { // Update SD Card indicator
            display->updateSdCard();
          }
        }
      }
      else
      { // Frame has been filterd
        if(filter == BEACON_FILTER_INVALID)
        { // Restert countdown on invalid frames if we were close to 0
          int currentCountDown = rtc->getCountDown();
          if(abs(currentCountDown)<=1)
          {
            rtc->startCountDown();
          }
          if(settings->getFrameSound()) hardware->getSoundManager()->playFrameSound(true);
        }
        else
        {
          if(settings->getFrameSound()) hardware->getSoundManager()->playFilteredFrameSound();
        }
        // Blink red led for filtered frames (valid or invalid)
        invalidFrameReceivedLedBlink();
      }
#ifdef WIFI
      if(wifiManagerClientCount() > 0)
      { // TODO
        wifiManagerSendFrameEvent(true,true);
      }
#endif    
    }
    else
    { // Frame preamble detected, but wrong start bytes
      // Restert countdown on invalid frames if we were close to 0
      int currentCountDown = rtc->getCountDown();
      if(abs(currentCountDown)<=1)
      {
        rtc->startCountDown();
      }
      // Error led
      invalidFrameReceivedLedBlink();
      Serial.println("Invalid frame !");
      if(settings->getFrameSound())
      {
        hardware->getSoundManager()->playInvalidFrameSound();
      }
#ifdef WIFI
      if(wifiManagerClientCount() > 0)
      {
        wifiManagerSendFrameEvent(false,true);
      }
#endif    
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
  if(rtc->countDownHasChanged())
  {
    display->updateTicker();
    if(settings->getCountDownSound())
    { // Play countdown beeps
      if(rtc->isAlmostLastCount())
      {
        hardware->getSoundManager()->playCountDownLowSound();
      }
      else if (rtc->isLastCount())
      {
        hardware->getSoundManager()->playCountDownHighSound();
      }
    }
    if(settings->getCountDownLeds())
    {
      if(rtc->isAlmostLastCount())
      { // Blue led 4 times
        blinkNotifLed();
      }
      else if (rtc->isLastCount())
      { // Red led once
        blinkErrorLed();
      }
    }
#ifdef WIFI
    if(wifiManagerClientCount() > 0)
    {
      wifiManagerSendTickerEvent(rtc->getCountDown(), rtc->getTimeString());
    }
#endif    
    if((rtc->getCountDown()==0) && settings->getReloadCountDown())
    {
      rtc->startCountDown();
    }
  }

    // Web and wifi
#ifdef WIFI
    if(wifiManagerHandleClient())
    {
      display->updateWifi();
    }
#endif
#ifdef BLUETOOTH
    if(bluetoothManagerHandleClient())
    {
      //TODO display->updateBluetooth();
    }
#endif
  //delay(5);
}


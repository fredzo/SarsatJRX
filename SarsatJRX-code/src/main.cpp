/*******************************
   SARSAT decodeur 
   Oled Display 128x64
   F4GMU
   Jan_2018
   
   Detresse/AutoTest      OK
   Longueur Trame         OK
   Pays                   OK
   4 protocoles           OK   
   Coord Nat location     OK
   Offset coord           OK
   Coord Sdt location     OK
   Offset coord           OK
   Coord User loction     OK   
   HEX Id                 OK
   Buzzer trame suivante  OK
   
   *Loop Test406 sur trame courte
   *Alarme Batterie
   *AUX 121.5
********************************/

#include <Arduino.h>

#include <Display.h>
#include <Location.h>
#include <Country.h>
#include <qrcode.h>
#include <Beacon.h>
#include <Power.h>
#include <Util.h>
#include <Samples.h>

// Enable RAM debuging
// #define DEBUG_RAM

// Enable decode debuging
//#define DEBUG_DECODE

// Header
#define HEADER_HEIGHT     20
#define HEADER_TEXT       "- SarsatJRX -"
#define HEADER_BOTTOM     24
#define HEADER_PAGES_TEMPLATE "%02d/%02d"
#define HEADER_PAGES_X    3
#define HEADER_PAGES_Y    (HEADER_HEIGHT-12)/2
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"
#define HEADER_POWER_X    DISPLAY_WIDTH-50
#define HEADER_POWER_Y    (HEADER_HEIGHT-12)/2
// Beacon info
#define LINE_HEIGHT       15
#define FRAME_MODE_LABEL  "Frame mode :"
#define FRAME_MODE_WIDTH  100
#define INFO_LABEL        "Info :"
#define INFO_LABEL_WIDTH  50
#define LOCATION_LABEL    "Location :"
#define HEX_ID_LABEL      "Hex ID:"
#define HEX_ID_WIDTH      60
#define DATA_LABEL        "Data :"
// MAPS and BEACON buttons
#define MAPS_BUTTON_X     246
#define MAPS_BUTTON_Y     HEADER_BOTTOM-1
#define MAPS_BUTTON_CAPTION "MAPS"
Display::Button mapsButton = Display::Button(MAPS_BUTTON_X,MAPS_BUTTON_Y,MAPS_BUTTON_CAPTION,Display::ButtonStyle::NORMAL);
#define BEACON_BUTTON_X   246
#define BEACON_BUTTON_Y   HEADER_BOTTOM+BUTTON_HEIGHT+1
#define BEACON_BUTTON_CAPTION "BEACON"
Display::Button beaconButton = Display::Button(BEACON_BUTTON_X,BEACON_BUTTON_Y,BEACON_BUTTON_CAPTION,Display::ButtonStyle::NORMAL);
// Navigation BUTTONS
#define PREVIONS_BUTTON_X   0
#define PREVIONS_BUTTON_Y   DISPLAY_HEIGHT-SMALL_BUTTON_HEIGHT-1
#define PREVIONS_BUTTON_CAPTION "<"
Display::Button previousButton = Display::Button(PREVIONS_BUTTON_X,PREVIONS_BUTTON_Y,PREVIONS_BUTTON_CAPTION,Display::ButtonStyle::SMALL);
#define NEXT_BUTTON_X   DISPLAY_WIDTH-SMALL_BUTTON_WIDTH-1
#define NEXT_BUTTON_Y   DISPLAY_HEIGHT-SMALL_BUTTON_HEIGHT-1
#define NEXT_BUTTON_CAPTION ">"
Display::Button nextButton = Display::Button(NEXT_BUTTON_X,NEXT_BUTTON_Y,NEXT_BUTTON_CAPTION,Display::ButtonStyle::SMALL);
// URL templates
#define MAPS_URL_TEMPLATE   "https://www.google.com/maps/search/?api=1&query=%s%%2C%s"
#define BEACON_URL_TEMPALTE "https://cryptic-earth-89063heroku-20.herokuapp.com/decoded/%s"

// Interupt pin : use digital pin 18
const int receiverPin = 18;
// Notification led : use digital pin 19
const int notificationPin = 19;

unsigned long microseconds;
unsigned long modDuration;
unsigned long frameStartTime;
int frameStartCount = 0;
bool frameStarted = false;
bool frameFinished = false;
bool lastModState = 1; // Start with 1
bool currentBitValue = 1; // Start with 1
byte frameParseState = 0;
byte bitCount;
byte byteCount;
byte currentByte;
const byte Nb_octet = 17; //3 + 15 **18 octets 1er FF  = octet 0**
byte frame[Beacon::SIZE];
float vout = 0.0;   // pour lecture tension batterie
float vin = 0.0;    // pour lecture tension batterie
int value = 0;      // pour lecture tension batterie

// Beacon list
#define BEACON_LIST_MAX_SIZE  20

Beacon* beacons[BEACON_LIST_MAX_SIZE];
int beaconsWriteIndex = -1;
int beaconsReadIndex = -1;
int beaconsSize = 0;
bool beaconsFull = false;


Display display;

#ifdef DEBUG_DECODE
  // For debug
  int events[512];
  int eventCount = 0;
#endif

void Test()
{ 
  // Frame format :
  // First Byte / Second Byte = Bit synchronization = 0xFF+0xFE
  // Third Byte = Frame synchrnization = 0xD0 for normal message / 0x2F for self-test message
  // First bit of fourth byte indicates the message format : 0 = short message (11 more bytes, i.e. 14 bytes in total) / 1 = long message (15 more bytes, i.e. 18 bytes in total)
  // Short Message :
  // |----------------------------------------------------------------------------------------|
  // | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4  
  // Long Message :
  if (frameParseState == 0) {
    if (currentByte == 255) {         // si 0xFF recu, frameParseState = 1
      frame[0] = currentByte;
      frameParseState = 1;
      bitCount = 0;
      byteCount = 1;
      currentByte = 0;
    }
    else {
      return;
    }
  }

  if (frameParseState == 1) {   //si 0xFF recu
    if (currentByte == 254) { // si 0xFE
      frame[1]= currentByte;
      frameParseState = 2;
      bitCount = 0;
      byteCount = 2;
      currentByte = 0;
    }
    else {
      return;
    }    
  }  
   
  if (frameParseState == 2) {   //si 0xFE recu
    if (currentByte == 208 || currentByte == 47) { // si 0xD0 ou 2F
      frame[2]= currentByte;
      frameParseState = 3;
      bitCount = 0;
      byteCount = 3;
      currentByte = 0;
    }
    else {
      return;
    }    
  }
  
  else if (frameParseState == 3) { //si 0xFE recu 
    if (bitCount == 7) 
    {                   //si nombre de bits = 8
      frame[byteCount] = currentByte;    //data dans octet numero xx
      byteCount ++;
      if(byteCount >= Beacon::SIZE)
      {
        frameFinished = true; 
      }
      currentByte = 0;
      bitCount = 0;
    }
    else if (bitCount < 7) {
      bitCount ++;
    }
  }
}

void resetFrameReading()
{ // Reset frame reading state machine
  byteCount = 0;     // repart pour trame suivante
  bitCount = 0;
  frameParseState = 0;
  currentByte = 0;
  lastModState = 1;
  currentBitValue = 1; // Frame always start with 1 bits
  frameStartCount = 0;
  frameStarted = false;
  frameFinished = false;
  // Clear frame content
  for ( byte i = 0; i < Beacon::SIZE; i++)
  {
    frame[i] = 0x00;
  }

#ifdef DEBUG_DECODE
  for ( int i = 0; i < 512; i++)
  {
    events[i] = 0UL;
  }
  eventCount = 0;
#endif
}

#define BIT_DURATION        2500UL                        // Bit duration = 2.5 ms = 2500 us
#define MOD_DURATION        BIT_DURATION/2                // Modulation duration 1.25 ms = 1250 us (half a bit)
#define TOLERANCE           625UL                         // Tolerate a 625 us time deviation
#define START_TOLERANCE     250UL                         // Tolerate a 250 us time deviation for frame start detection
#define FRAME_START_SIZE    28                            // Frame synchronisation pattern is 15 bits i.e. at least 28 modulations
#define START_PHASE_START   MOD_DURATION-START_TOLERANCE  // 1250 - 250 = 1000 us
#define START_PHASE_END     MOD_DURATION+START_TOLERANCE  // 1250 + 250 = 1500 us
#define SAME_PHASE_START    MOD_DURATION-TOLERANCE        // 1250 - 625 = 625 us
#define SAME_PHASE_END      MOD_DURATION+TOLERANCE        // 1250 + 625 = 1875 us
#define CHANGE_PHASE_START  (2*MOD_DURATION)-TOLERANCE    // 2500 - 625 = 1875 us
#define CHANGE_PHASE_END    (2*MOD_DURATION)+TOLERANCE    // 2500 + 625 = 3125 us

/***********************************************
  Extraction des bits
***********************************************/
void analyze(void)
{
  modDuration = micros() - microseconds;
#ifdef DEBUG_DECODE
  if(eventCount<512)
  {
    events[eventCount]=modDuration;
    eventCount++;
  }
#endif

  if(!frameStarted)
  { // Monitor closely first 1250-ish us modulations
    microseconds = micros();
    if(modDuration >= START_PHASE_START && modDuration < START_PHASE_END)
    { // This looks like a frame start
      frameStartCount ++;
      currentByte = currentByte << 1 | 1;
      Test();
      if(frameStartCount >= FRAME_START_SIZE)
      {
        frameStarted = true;
        frameStartTime = millis();
      }
    }
    else
    {
      frameStartCount = 0;
    }
  }
  else
  { // We are in a frame
    if(frameFinished)
    { // Frame finished, ignore future bits
      return;
    }
    if(modDuration <= SAME_PHASE_START)
    { // Debounce too short changes
      return;
    }
    microseconds = micros();
    if(modDuration >= SAME_PHASE_START && modDuration < SAME_PHASE_END)
    { // Same phase
      if(lastModState)
      { // New modulation
        lastModState = 0;
        // Store new bit and lauch parsing
        currentByte = currentByte << 1 | currentBitValue;
        Test();
      }
      lastModState = !lastModState;
    }
    else if(modDuration >= CHANGE_PHASE_START && modDuration < CHANGE_PHASE_END)
    { // Phase change => bit value change
      currentBitValue = !currentBitValue;
      // Store new bit and lauch parsing
      currentByte = currentByte << 1 | currentBitValue;
      lastModState = 0;
      Test();
    }
    else 
    { // We have missed changes
      int missedModulation = modDuration / MOD_DURATION;
      // Check if it's an even number of modulations
      int missedBits = missedModulation/2;
      // Let's assume we received n times the same value 
      for(int i = 0 ; i < missedBits ; i++)
      {
        currentByte = currentByte << 1 | currentBitValue;
        Test();
      }
      bool odd = ((missedModulation%2) != 0);
      if(odd) 
      { // We have a phase change
        currentBitValue = !currentBitValue;
        // Store new bit and lauch parsing
        currentByte = currentByte << 1 | currentBitValue;
        lastModState = 0;
        Test();
      }
    }
  }
}

static const int QR_VERSION = 6;
void generateQrCode(QRCode* qrcode, Beacon* beacon, bool isMaps)
{ // Version 6 (41x41) allows 154 alphanumeric characters with medium error correcion
  // For some reason, to have the qr code library work properly, the version passed to getBufferSize needs to be one step ahed the actual version...
  uint8_t qrcodeData[qrcode_getBufferSize(QR_VERSION+1)];
  char buffer[128];
  if(isMaps)
  {
    beacon->location.formatFloatLocation(buffer,MAPS_URL_TEMPLATE);
  }
  else
  {
    sprintf(buffer,BEACON_URL_TEMPALTE, toHexString(beacon->frame, false, 3, beacon->longFrame ? 18 : 14).c_str());
  }
  Serial.println(buffer);
	qrcode_initText(qrcode, qrcodeData, QR_VERSION, ECC_MEDIUM, buffer);
}

static const int MODULE_SIZE = 3;
void displayQrCode (QRCode* qrcode, int xPos, int yPos)
{
  display.setCursor(xPos,yPos);
  display.setColor(Display::Color::WHITE);
  // Add a padding around the QR code
  display.fillRectangle((qrcode->size+2)*MODULE_SIZE,(qrcode->size+2)*MODULE_SIZE);
  display.setColor(Display::Color::BLACK);
  xPos += MODULE_SIZE;
  yPos += MODULE_SIZE;
  for (int y = 0; y < qrcode->size; y++) 
  {
      for (int x = 0; x < qrcode->size; x++) 
      {
          if (qrcode_getModule(qrcode, x, y)) 
          {
              display.setCursor(xPos + (x * MODULE_SIZE),yPos + (y * MODULE_SIZE));
							display.fillRectangle(MODULE_SIZE, MODULE_SIZE);
          }
      }
  }
}

void drawQrCode(bool isMaps)
{
    QRCode qrCode;
    generateQrCode(&qrCode,beacons[beaconsReadIndex],isMaps);
    int xPos = display.getWidth()-((qrCode.size+3)*MODULE_SIZE);
    int yPos = display.getHeight()-((qrCode.size+3)*MODULE_SIZE);
    displayQrCode(&qrCode,xPos,yPos);
}

#ifdef DEBUG_RAM
int freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0  
    ? (int)&__heap_start : (int) __brkval);  
}
#endif

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
  Beacon* beacon = new Beacon(frame);
  // Add beacon to the list
  beacons[beaconsWriteIndex] = beacon;
  // Move to last received
  beaconsReadIndex = beaconsWriteIndex;

#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
}

/*****************************
  Routine autotest 406
******************************/
void updateDisplay()
{
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  // Refresh screen
  display.setBackgroundColor(Display::Color::DARK_GREY);
  display.clearDisplay();

  // Header
  display.setCursor(0, 0);
  // Draw header background
  display.setColor(Display::Color::GREY);
  display.setBackgroundColor(Display::Color::GREY);
  display.fillRoundRectangle(display.getWidth()-1,HEADER_HEIGHT);
  display.setColor(Display::Color::PURPLE);
  display.drawRoundRectangle(display.getWidth()-1,HEADER_HEIGHT);
  // Header text
  display.setColor(Display::Color::LIGHT_BLUE);
  display.setCursor(0, 3);
  display.setFontSize(Display::FontSize::LARGE);
  display.centerText(HEADER_TEXT,display.getWidth());
  display.println(HEADER_TEXT);
  Serial.print(HEADER_TEXT);
  // Header pages
  display.setFontSize(Display::FontSize::SMALL);
  display.setColor(Display::Color::GREEN);
  display.setCursor(HEADER_PAGES_X, HEADER_PAGES_Y);
  char buffer[32];
  // Rotating index based on beacon list max size and position of last read frame
  int displayIndex = ((beaconsSize-1 + beaconsReadIndex - beaconsWriteIndex) % BEACON_LIST_MAX_SIZE)+1;
  sprintf(buffer,HEADER_PAGES_TEMPLATE,displayIndex,beaconsSize);
  display.println(buffer);
  Serial.println(buffer);
  // Header power
  display.setCursor(HEADER_POWER_X, HEADER_POWER_Y);
  char powerString[8];
  getVccStringValue(powerString);
  sprintf(buffer,HEADER_POWER_TEMPLATE,powerString);
  display.println(buffer);
  Serial.println(buffer);


  int currentY = HEADER_BOTTOM;
  // For the rest of the screen
  display.setColor(Display::Color::BEIGE);
  display.setBackgroundColor(Display::Color::DARK_GREY);

  if(beaconsReadIndex<0)
  { // Nothing to display
    return;
  }
  // Get current beacon
  Beacon* beacon = beacons[beaconsReadIndex];
  // Frame mode
  String frameMode;
  if (beacon->frameMode == Beacon::FrameMode::SELF_TEST) 
  {  // Self-test message frame synchronisation byte
    frameMode = F("Self-test 406");
  }
  else if (beacon->frameMode == Beacon::FrameMode::NORMAL) 
  { // Normal message frame synchronisation byte
    frameMode = F("Distress 406");
  }
  else
  { // Unknown fram format
    frameMode = F("Unknown 406");
  }
  display.setCursor(0, currentY);
  display.setColor(Display::Color::GREEN);
  display.println(FRAME_MODE_LABEL);
  display.setColor(Display::Color::BEIGE);
  display.setCursor(FRAME_MODE_WIDTH, currentY);
  display.println(frameMode);
  Serial.println(frameMode);
  currentY+=LINE_HEIGHT;

  // Info           
  display.setCursor(0, currentY);
  display.setColor(Display::Color::GREEN);
  display.println(INFO_LABEL);
  display.setColor(Display::Color::BEIGE);
  // Protocol name
  display.setCursor(INFO_LABEL_WIDTH, currentY);
  if(beacon->protocol->isUnknown())
  {
    // Unknwon protocol (xxxx)
    sprintf(buffer,"%s (%lu)",beacon->getProtocolName().c_str(),beacon->protocolCode);
    display.println(buffer);
    Serial.println(buffer);   
  }
  else
  {
    display.println(beacon->getProtocolName());
    Serial.println(beacon->getProtocolName());   
  }
  // Protocol desciption
  currentY+=LINE_HEIGHT;
  display.setCursor(0, currentY);
  display.println(beacon->getProtocolDesciption());
  Serial.println(beacon->getProtocolDesciption());   
  currentY+=LINE_HEIGHT;

  // Location
  display.setCursor(0, currentY);
  display.setColor(Display::Color::GREEN);
  display.println(LOCATION_LABEL);
  display.setColor(Display::Color::BEIGE);
  currentY+=LINE_HEIGHT;
  // Country
  display.setCursor(0, currentY);
  String country = beacon->country.toString();
  display.println(country);
  Serial.println(country);
  currentY+=LINE_HEIGHT;

  // Coordinates
  bool locationKnown = !beacon->location.isUnknown(); 
  if (beacon->longFrame) 
  { // Long frame
    display.setCursor(0, currentY); 
    String locationSexa = beacon->location.toString(true);
    String locationDeci = beacon->location.toString(false);
    display.println(locationSexa);
    Serial.println(locationSexa);
    currentY+=LINE_HEIGHT;
    if(locationKnown)
    {
      display.setCursor(0, currentY); 
      display.println(locationDeci);
      Serial.println(locationDeci);
    }
    currentY+=LINE_HEIGHT;
  }
  else 
  { // Short frame
    display.setCursor(5, currentY);
    display.println("22 HEX. No location");
    currentY+=LINE_HEIGHT;
  }

  // Hex ID
  display.setCursor(0, currentY);
  display.setColor(Display::Color::GREEN);
  display.println(HEX_ID_LABEL);
  display.setColor(Display::Color::BEIGE);
  display.setCursor(HEX_ID_WIDTH, currentY);
  // "Id = 0x1122334455667788"
  uint32_t msb = beacon->identifier >> 32;
  uint32_t lsb = beacon->identifier;
  sprintf(buffer,"%07lX%08lX",msb,lsb);
  display.println(buffer);
  Serial.println(buffer);   
  currentY+=LINE_HEIGHT;

  // Data
  display.setCursor(0, currentY);
  display.setColor(Display::Color::GREEN);
  display.println(DATA_LABEL);
  display.setColor(Display::Color::BEIGE);
  currentY+=LINE_HEIGHT;
  if (beacon->longFrame) 
  { // Long frame
    // HEX ID 30 Hexa
    display.setCursor(0, currentY);
    display.println(toHexString(beacon->frame,true,3,11));
    currentY+=LINE_HEIGHT;
    display.setCursor(0, currentY);
    display.println(toHexString(beacon->frame,true,11,18));
  }
  else 
  { // Short frame
    display.setCursor(0, currentY);  
    // HEX ID 22 Hexa bit 26 to 112
    display.println(toHexString(beacon->frame,true,3,11));
    currentY+=LINE_HEIGHT;
    display.setCursor(0, currentY);
    display.println(toHexString(beacon->frame,true,11,14));
  }

  // Maps button
  mapsButton.enabled = locationKnown;
  display.drawButton(mapsButton);

  // Beacon button
  beaconButton.enabled = true;
  display.drawButton(beaconButton);

  // Navigation buttons
  display.drawButton(previousButton);
  display.drawButton(nextButton);

#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif

}


void ledblink()
{
  digitalWrite(notificationPin, HIGH);  // Clignotement LED Trame décodée
  delay(100);
  digitalWrite(notificationPin, LOW); 
}

void readNextSampleFrame()
{ // Read the frame content
  readNextSample(frame);
  // Tell the state machine that we have a complete frame
  byteCount = Nb_octet;
}

void setup()
{
  pinMode(receiverPin, INPUT);          // Detection stage inpit
  pinMode(notificationPin, OUTPUT);     // Notification led output
  // pinMode(16, OUTPUT);                  // Buzzer output
  
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(receiverPin), analyze, CHANGE);  // interruption sur Rise et Fall

  display.setup();
  previousButton.enabled = true;
  nextButton.enabled = true;

  readNextSampleFrame();
  Serial.println("### Boot complete !");
}

void loop()
{
  //Serial.print(".");
  if(display.touchAvailable())
  {
    int x = display.getTouchX();
    int y = display.getTouchY();
    if(x>=0 && y>=0)
    {
      Serial.println(x);
      Serial.println(y);
      if(mapsButton.contains(x,y))
      {
        if(mapsButton.enabled)
        {
          mapsButton.pressed = true;
          display.drawButton(mapsButton);
          drawQrCode(true);
          mapsButton.pressed = false;
          display.drawButton(mapsButton);
        }
      }
      else if(beaconButton.contains(x,y))
      {
        if(beaconButton.enabled)
        {
          beaconButton.pressed = true;
          display.drawButton(beaconButton);
          drawQrCode(false);
          beaconButton.pressed = false;
          display.drawButton(beaconButton);
        }
      }
      else if(previousButton.contains(x,y))
      {
        if(previousButton.enabled)
        {
          previousButton.pressed = true;
          display.drawButton(previousButton);
          beaconsReadIndex--;
          if(beaconsReadIndex<0)
          {
            beaconsReadIndex = beaconsSize-1;
          }
          updateDisplay();
          previousButton.pressed = false;
          display.drawButton(previousButton);
        }
      }
      else if(nextButton.contains(x,y))
      {
        if(nextButton.enabled)
        {
          nextButton.pressed = true;
          display.drawButton(nextButton);
          beaconsReadIndex++;
          if(beaconsReadIndex>=beaconsSize)
          {
            beaconsReadIndex = 0;
          }
          updateDisplay();
          nextButton.pressed = false;
          display.drawButton(nextButton);
        }
      }
      else
      {
        //Serial.print("Read frame #");
        //Serial.println(curFrame);
        readNextSampleFrame();
      }
    }
  }
  // If frameParseState > 0, we are reading an frame, if more thant 500ms elapsed (a frame should be 144x2.5ms = 360 ms max)
  bool frameTimeout = (frameStarted && ((millis()-frameStartTime) > 500 ));
  if ((byteCount == Nb_octet) || frameTimeout)
  {
    // Pour debug 
    if(frameTimeout)
    {
      Serial.println("Frame timeout !");
    }
    for ( byte i = 0; i < Nb_octet; i++) // RAW data
    {
      if (frame[i] < 16)
      Serial.print('0');
      Serial.print (frame[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");

#ifdef DEBUG_DECODE
    for ( int i = 0; i < eventCount; i++)
    {
      Serial.println(events[i],DEC);
    }
    Serial.println("");
#endif    

    if (((frame[1] == 0xFE) && (frame[2] == 0xD0)) || ((frame[1] == 0xFE) && (frame[2] == 0x2F)))// 0XFE/0x2F for normal mode, 0xFE/0xD0  for autotest
    {
      readBeacon();
      updateDisplay();
      ledblink();
    } 
    // Reset frame decoding
    resetFrameReading();
  }
}


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
#include <Decoder.h>

// Enable RAM debuging
#define DEBUG_RAM

// Enable serial out
#define SERIAL_OUT

// Header
#define HEADER_HEIGHT     40
#define HEADER_TEXT       F("- SarsatJRX -")
#define HEADER_BOTTOM     HEADER_HEIGHT+8
#define HEADER_PAGES_TEMPLATE "%02d/%02d"
#define HEADER_PAGES_X    6
#define HEADER_PAGES_Y    (HEADER_HEIGHT-12)/2
#define HEADER_BUTTON_LEFT    DISPLAY_WIDTH/3
#define HEADER_BUTTON_RIGHT   (2*DISPLAY_WIDTH)/3
#define HEADER_BUTTON_BOTTOM  2*HEADER_HEIGHT
// Header LEDS
#define LED_SIG_1_X       DISPLAY_WIDTH-(4*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_1_Y       HEADER_HEIGHT/2
Display::Led ledSig1 =    Display::Led(LED_SIG_1_X,LED_SIG_1_Y,Display::LedColor::Green);
#define LED_SIG_2_X       DISPLAY_WIDTH-(3*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_2_Y       LED_SIG_1_Y
Display::Led ledSig2 =    Display::Led(LED_SIG_2_X,LED_SIG_2_Y,Display::LedColor::Green);
#define LED_SIG_IN_FRAME_X      DISPLAY_WIDTH-(2*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_IN_FRAME_Y      LED_SIG_1_Y
Display::Led ledInFrame =       Display::Led(LED_SIG_IN_FRAME_X,LED_SIG_IN_FRAME_Y,Display::LedColor::Red);
#define LED_SIG_FRAME_R_X       DISPLAY_WIDTH-(1*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_FRAME_R_Y       LED_SIG_1_Y
Display::Led ledFrameReceived = Display::Led(LED_SIG_FRAME_R_X,LED_SIG_FRAME_R_Y,Display::LedColor::Blue);
// Power
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"
#define HEADER_POWER_X    LED_SIG_1_X-100
#define HEADER_POWER_Y    (HEADER_HEIGHT-12)/2

// Footer
#define FOOTER_LABEL_X      DISPLAY_WIDTH/2
#define FOOTER_LABEL_Y      DISPLAY_HEIGHT-SMALL_BUTTON_HEIGHT+24
#define FOOTER_WAIT_LABEL   F("Waiting for the wave...")
#define FOOTER_FRAME_LABEL  F("Frame reveived !")
#define FOOTER_SPINNER_X    SMALL_BUTTON_WIDTH*1.5
#define FOOTER_SPINNER_Y    DISPLAY_HEIGHT-(SMALL_BUTTON_HEIGHT/2)
#define FOOTER_RADIUS_MAX   SMALL_BUTTON_HEIGHT/2-20
#define FOOTER_RADIUS_MIN   FOOTER_RADIUS_MAX-5

// Beacon info
#define LINE_HEIGHT         22
#define FRAME_MODE_LABEL    F("Frame mode :")
#define FRAME_MODE_WIDTH    180
#define INFO_LABEL          F("Info :")
#define INFO_LABEL_WIDTH    100
#define SERIAL_LABEL        F("Serial # :")
#define SERIAL_LABEL_WIDTH  150
#define MAIN_LABEL          F("Main loc. device :")
#define MAIN_LABEL_WIDTH    260
#define AUX_LABEL           F("Aux. loc. device :")
#define AUX_LABEL_WIDTH     260
#define LOCATION_LABEL      F("Location :")
#define HEX_ID_LABEL        F("Hex ID:")
#define HEX_ID_WIDTH        120
#define DATA_LABEL          F("Data :")
#define DATA_LABEL_WIDTH    100
#define BCH1_OK_LABEL       F("BCH1-OK")
#define BCH1_KO_LABEL       F("BCH1-KO")
#define BCH2_OK_LABEL       F("BCH2-OK")
#define BCH2_KO_LABEL       F("BCH2-KO")
#define BCH_LABEL_WIDTH     120
// MAPS and BEACON buttons
#define MAPS_BUTTON_X     DISPLAY_WIDTH-BUTTON_WIDTH
#define MAPS_BUTTON_Y     HEADER_BOTTOM-1
#define MAPS_BUTTON_CAPTION "MAPS"
Display::Button mapsButton = Display::Button(MAPS_BUTTON_X,MAPS_BUTTON_Y,MAPS_BUTTON_CAPTION,Display::ButtonStyle::NORMAL);
#define BEACON_BUTTON_X   MAPS_BUTTON_X
#define BEACON_BUTTON_Y   HEADER_BOTTOM+BUTTON_HEIGHT+4
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

// Beacon list
#define BEACON_LIST_MAX_SIZE  20

Beacon* beacons[BEACON_LIST_MAX_SIZE];
int beaconsWriteIndex = -1;
int beaconsReadIndex = -1;
int beaconsSize = 0;
bool beaconsFull = false;


Display display;

static const int QR_VERSION = 6;
void generateQrCode(QRCode* qrcode, Beacon* beacon, bool isMaps)
{ // Version 6 (41x41) allows 154 alphanumeric characters with medium error correcion
  // For some reason, to have the qr code library work properly, the version passed to getBufferSize needs to be one step ahed the actual version...
  uint8_t qrcodeData[qrcode_getBufferSize(QR_VERSION+2)];
  char buffer[128];
  if(isMaps)
  {
    beacon->location.formatFloatLocation(buffer,MAPS_URL_TEMPLATE);
  }
  else
  {
    sprintf(buffer,BEACON_URL_TEMPALTE, toHexString(beacon->frame, false, 3, beacon->longFrame ? 18 : 14).c_str());
  }
  #ifdef SERIAL_OUT 
  Serial.println(buffer); 
  #endif
	qrcode_initText(qrcode, qrcodeData, QR_VERSION, ECC_MEDIUM, buffer);
}

static const int MODULE_SIZE = 4;
void drawQrCode(bool isMaps)
{
    QRCode qrCode;
    generateQrCode(&qrCode,beacons[beaconsReadIndex],isMaps);
    int xPos = display.getWidth()-((qrCode.size+3)*MODULE_SIZE);
    int yPos = display.getHeight()-((qrCode.size+3)*MODULE_SIZE);
    display.setCursor(xPos,yPos);
    display.drawQrCode(&qrCode,MODULE_SIZE);
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
#define LED_BLINK_TIME 100 // 100 ms
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
float powerValue = 0;

void updatePowerValueHeader()
{  // Power header
  display.setTextColors(Display::Color::LightGreen,Display::Color::Grey);
  display.setFontSize(Display::FontSize::SMALL);
  display.setCursor(HEADER_POWER_X, HEADER_POWER_Y);
  char powerString[8];
  getVccStringValue(powerString);
  char buffer[8];
  sprintf(buffer,HEADER_POWER_TEMPLATE,powerString);
  display.println(buffer);
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
  { // Flip ledSig1 and ledSig2 according to decoder input stage state
    lastInputState = newInputState;
    ledSig1.on = newInputState;
    ledSig2.on = !newInputState;
    drawledSig1 = true;
    drawledSig2 = true;
    if(ledSig1.on) ledSig1StartBlinkTime = now;
    if(ledSig2.on) ledSig2StartBlinkTime = now;
  }
  else
  { // Check if we have to turn decoder input leds off
    if(ledSig1.on && ((now - ledSig1StartBlinkTime) > LED_BLINK_TIME))
    {
      ledSig1.on = false;
      drawledSig1 = true;
    }
    if(ledSig2.on && ((now - ledSig2StartBlinkTime) > LED_BLINK_TIME))
    {
      ledSig2.on = false;
      drawledSig2 = true;
    }
  }
  if(ledInFrame.on != isFrameStarted())
  { // Check if frame started state changed
    ledInFrame.on = isFrameStarted();
    drawledInFrame = true;
  }
  if(ledFrameReceived.on != ledFrameReceivedOn)
  { // Check if frame received state changed
    ledFrameReceived.on = ledFrameReceivedOn;
    drawledFrameReceived = true;
  }
  if(drawledSig1) display.drawLed(ledSig1);
  if(drawledSig2) display.drawLed(ledSig2);
  if(drawledInFrame) display.drawLed(ledInFrame);
  if(drawledFrameReceived) display.drawLed(ledFrameReceived);
}

void updateHeader()
{ // Check for power value update
  float newValue = getVccValue();
  if(abs(newValue - powerValue) > 0.07)
  {
    powerValue = newValue;
    updatePowerValueHeader();
  }
  updateLedHeader(false);
}

void drawHeader()
{
  // Header
  display.setCursor(0, 0);
  // Draw header background
  display.setColor(Display::Color::Grey);
  display.fillRoundRectangle(display.getWidth()-1,HEADER_HEIGHT);
  display.setColor(Display::Color::Purple);
  display.drawRoundRectangle(display.getWidth()-1,HEADER_HEIGHT);
  // Header text
  display.setTextColors(Display::Color::LightBlue,Display::Color::Grey);
  display.setCursor(DISPLAY_WIDTH/2, 3);
  display.setFontSize(Display::FontSize::LARGE);
  display.centerText(HEADER_TEXT);
  display.println(HEADER_TEXT);
}

// Footer management
unsigned long lastFooterUpdateTime;
bool footerShowingFrameReceived = false;
bool footerShowingWait = false;
bool footerShowingSpinner = false;
int spinnerPosition = 0;
#define FOOTER_UPDATE_TIME 10
#define FOOTER_FRAME_RECEIVED_TIME 2000

void updateSpinner(bool show)
{
  if(show)
  {
    if(spinnerPosition < 360)
    {
      display.setCursor(FOOTER_SPINNER_X,FOOTER_SPINNER_Y);
      display.setColor(Display::Color::LightBlue);
      display.fillArc(FOOTER_RADIUS_MIN,FOOTER_RADIUS_MAX,0,spinnerPosition);
    }
    else if(spinnerPosition < 360*2)
    {
      display.setCursor(FOOTER_SPINNER_X,FOOTER_SPINNER_Y);
      display.setColor(Display::Color::DarkGrey);
      display.fillArc(FOOTER_RADIUS_MIN,FOOTER_RADIUS_MAX,0,spinnerPosition-360);
    }
    else
    {
      spinnerPosition = 0;
    }
    spinnerPosition+=4;
  }
  else
  {
    spinnerPosition = 0;
    display.setCursor(FOOTER_SPINNER_X,FOOTER_SPINNER_Y);
    display.setColor(Display::Color::DarkGrey);
    display.fillArc(FOOTER_RADIUS_MAX,FOOTER_RADIUS_MIN,0,360);
  }
  footerShowingSpinner = show;
}

void showWaiting(bool show)
{
    display.setTextColors(show ? Display::Color::LightBlue : Display::Color::DarkGrey,Display::Color::DarkGrey);
    display.setFontSize(Display::FontSize::LARGE);
    // Center not working with large font (not fixed with)
    display.setCursor(SMALL_BUTTON_WIDTH*2, FOOTER_LABEL_Y);
    //display.centerText(FOOTER_WAIT_LABEL);
    display.println(FOOTER_WAIT_LABEL);
    footerShowingWait = show;
}

void showFrameReceived(bool show)
{
    display.setTextColors(show ? Display::Color::Red : Display::Color::DarkGrey,Display::Color::DarkGrey);
    display.setFontSize(Display::FontSize::LARGE);
    display.setCursor(FOOTER_LABEL_X, FOOTER_LABEL_Y);
    display.centerText(FOOTER_FRAME_LABEL);
    display.println(FOOTER_FRAME_LABEL);
    footerShowingFrameReceived = show;
}

void updateFooter(bool frameReceived)
{  
  unsigned long now = millis();
  if(footerShowingFrameReceived)
  {
    if((now - lastFooterUpdateTime) > FOOTER_FRAME_RECEIVED_TIME)
    {
      lastFooterUpdateTime = now;
      showFrameReceived(false);
      showWaiting(true);
    }
  }
  else
  {
    if(frameReceived)
    {
      lastFooterUpdateTime = now;
      showWaiting(false);
      updateSpinner(false);
      showFrameReceived(true);
    }
    else if ((now - lastFooterUpdateTime) > FOOTER_UPDATE_TIME)
    {
      lastFooterUpdateTime = now;
      if(!footerShowingWait)
      {
        showWaiting(true);
      }
      updateSpinner(true);
    }
  }
}

void updateDisplay()
{
#ifdef DEBUG_RAM
  Serial.println(freeRam());
#endif
  // Refresh screen
  display.clearDisplay(true);

#ifdef SERIAL_OUT 
  Serial.print(HEADER_TEXT);
#endif  
  // Header pages
  display.setFontSize(Display::FontSize::SMALL);
  display.setTextColors(Display::Color::LightGreen,Display::Color::Grey);
  display.setCursor(HEADER_PAGES_X, HEADER_PAGES_Y);
  char buffer[32];
  // Rotating index based on beacon list max size and position of last read frame
  int displayIndex = ((beaconsSize-1 + beaconsReadIndex - beaconsWriteIndex) % BEACON_LIST_MAX_SIZE)+1;
  sprintf(buffer,HEADER_PAGES_TEMPLATE,displayIndex,beaconsSize);
  display.println(buffer);
#ifdef SERIAL_OUT 
  Serial.println(buffer);
#endif
  // Header power and leds
  updatePowerValueHeader();
  updateLedHeader(true);

  int currentY = HEADER_BOTTOM;
  // For the rest of the screen
  display.setTextColors(Display::Color::Beige,Display::Color::DarkGrey);

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
  display.setTextColor(Display::Color::LightGreen);
  display.println(FRAME_MODE_LABEL);
  display.setTextColor(Display::Color::Beige);
  display.setCursor(FRAME_MODE_WIDTH, currentY);
  display.println(frameMode);
#ifdef SERIAL_OUT 
  Serial.println(frameMode);
#endif
  currentY+=LINE_HEIGHT;

  // Info           
  display.setCursor(0, currentY);
  display.setTextColor(Display::Color::LightGreen);
  display.println(INFO_LABEL);
  display.setTextColor(Display::Color::Beige);
  // Protocol name
  display.setCursor(INFO_LABEL_WIDTH, currentY);
  if(beacon->protocol->isUnknown())
  {
    // Unknwon protocol (xxxx)
    sprintf(buffer,"%s (%lu)",beacon->getProtocolName().c_str(),beacon->protocolCode);
    display.println(buffer);
  #ifdef SERIAL_OUT 
    Serial.println(buffer);   
  #endif
  }
  else
  {
    display.println(beacon->getProtocolName());
  #ifdef SERIAL_OUT 
    Serial.println(beacon->getProtocolName());   
  #endif
  }
  // Protocol desciption
  currentY+=LINE_HEIGHT;
  display.setCursor(0, currentY);
  display.println(beacon->getProtocolDesciption());
#ifdef SERIAL_OUT 
  Serial.println(beacon->getProtocolDesciption());   
#endif
  currentY+=LINE_HEIGHT;
  if(beacon->hasAdditionalData)
  { // Additionnal protocol data
    display.setCursor(0, currentY);
    display.println(beacon->additionalData);
  #ifdef SERIAL_OUT 
    Serial.println(beacon->additionalData);   
  #endif
    currentY+=LINE_HEIGHT;
  }

  if(beacon->hasSerialNumber)
  { // Serial number
    display.setCursor(0, currentY);
    display.setTextColor(Display::Color::LightGreen);
    display.println(SERIAL_LABEL);
    display.setTextColor(Display::Color::Beige);
    display.setCursor(SERIAL_LABEL_WIDTH, currentY);
    display.println(beacon->serialNumber);
  #ifdef SERIAL_OUT 
    Serial.print(SERIAL_LABEL);   
    Serial.println(beacon->serialNumber);   
  #endif
    currentY+=LINE_HEIGHT;
  }

  // Location
  display.setCursor(0, currentY);
  display.setTextColor(Display::Color::LightGreen);
  display.println(LOCATION_LABEL);
  display.setTextColor(Display::Color::Beige);
  currentY+=LINE_HEIGHT;
  // Country
  display.setCursor(0, currentY);
  String country = beacon->country.toString();
  display.println(country);
#ifdef SERIAL_OUT 
  Serial.println(country);
#endif
  currentY+=LINE_HEIGHT;

  // Coordinates
  bool locationKnown = !beacon->location.isUnknown(); 
  if (beacon->longFrame) 
  { // Long frame
    display.setCursor(0, currentY); 
    String locationSexa = beacon->location.toString(true);
    String locationDeci = beacon->location.toString(false);
    display.println(locationSexa);
  #ifdef SERIAL_OUT 
    Serial.println(locationSexa);
  #endif
    currentY+=LINE_HEIGHT;
    if(locationKnown)
    {
      display.setCursor(0, currentY); 
      display.println(locationDeci);
    #ifdef SERIAL_OUT 
      Serial.println(locationDeci);
    #endif
    }
    currentY+=LINE_HEIGHT;
  }
  else 
  { // Short frame
    display.setCursor(5, currentY);
    display.println("22 HEX. No location");
    currentY+=LINE_HEIGHT;
  }

  if(beacon->hasMainLocatingDevice())
  { // Main locating device
    display.setCursor(0, currentY);
    display.setTextColor(Display::Color::LightGreen);
    display.println(MAIN_LABEL);
    display.setTextColor(Display::Color::Beige);
    display.setCursor(MAIN_LABEL_WIDTH, currentY);
    display.println(beacon->getMainLocatingDeviceName());
  #ifdef SERIAL_OUT 
    Serial.print(MAIN_LABEL);   
    Serial.println(beacon->getMainLocatingDeviceName());   
  #endif
    currentY+=LINE_HEIGHT;
  }

  if(beacon->hasAuxLocatingDevice())
  { // Auxiliary locating device
    display.setCursor(0, currentY);
    display.setTextColor(Display::Color::LightGreen);
    display.println(AUX_LABEL);
    display.setTextColor(Display::Color::Beige);
    display.setCursor(AUX_LABEL_WIDTH, currentY);
    display.println(beacon->getAuxLocatingDeviceName());
  #ifdef SERIAL_OUT 
    Serial.print(AUX_LABEL);   
    Serial.println(beacon->getAuxLocatingDeviceName());   
  #endif
    currentY+=LINE_HEIGHT;
  }

  // Hex ID
  display.setCursor(0, currentY);
  display.setTextColor(Display::Color::LightGreen);
  display.println(HEX_ID_LABEL);
  display.setTextColor(Display::Color::Beige);
  display.setCursor(HEX_ID_WIDTH, currentY);
  // "Id = 0x1122334455667788"
  uint32_t msb = beacon->identifier >> 32;
  uint32_t lsb = beacon->identifier;
  sprintf(buffer,"%07lX%08lX",msb,lsb);
  display.println(buffer);
#ifdef SERIAL_OUT 
  Serial.println(buffer);   
#endif  
  currentY+=LINE_HEIGHT;

  // Data
  display.setCursor(0, currentY);
  display.setTextColor(Display::Color::LightGreen);
  display.println(DATA_LABEL);
  // Append BCH values before frame data
  display.setTextColor(beacon->isBch1Valid() ? Display::Color::Green : Display::Color::Red);
  display.setCursor(DATA_LABEL_WIDTH, currentY);
  display.println(beacon->isBch1Valid() ? BCH1_OK_LABEL : BCH1_KO_LABEL);
  if(beacon->longFrame) 
  { // No second proteced field in short frames
    display.setTextColor(beacon->isBch2Valid() ? Display::Color::Green : Display::Color::Red);
    display.setCursor(DATA_LABEL_WIDTH+BCH_LABEL_WIDTH, currentY);
    display.println(beacon->isBch2Valid() ? BCH2_OK_LABEL : BCH2_KO_LABEL);
  }
#ifdef SERIAL_OUT 
  if(!beacon->isBch1Valid())
  {
    Serial.println("Wrong BCH1 value :");
    Serial.print("Found    :");
    Serial.println(beacon->bch1,2);
    Serial.print("Expected :");
    Serial.println(beacon->computedBch1,2);
  }
  if(beacon->longFrame && !beacon->isBch2Valid())
  {
    Serial.println("Wrong BCH2 value :");
    Serial.print("Found    :");
    Serial.println(beacon->bch2,2);
    Serial.print("Expected :");
    Serial.println(beacon->computedBch2,2);
  }
#endif

  display.setTextColor(Display::Color::Beige);
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


void readNextSampleFrame()
{ // Read the frame content
  readNextSample(getFrame());
  // Tell the state machine that we have a complete frame
  setFrameComplete(true);
}

#ifdef TOUCH_CAL
int16_t w = -1, h = -1;
int16_t point_x[4] = {-1};
int16_t point_y[4] = {-1};
int16_t current_point = -1, next_point = 0;
int16_t touched_x[4] = {-1};
int16_t touched_y[4] = {-1};
bool ctouch_swap_xy = false;
int16_t ctouch_map_x1 = -1;
int16_t ctouch_map_x2 = -1;
int16_t ctouch_map_y1 = -1;
int16_t ctouch_map_y2 = -1;
#endif


void setup()
{
  pinMode(receiverPin, INPUT);          // Detection stage inpit
  pinMode(notificationPin, OUTPUT);     // Notification led output
  // pinMode(16, OUTPUT);                  // Buzzer output
  
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(receiverPin), analyze, CHANGE);  // interruption sur Rise et Fall

  display.setup(Display::Color::DarkGrey);

#ifdef TOUCH_CAL
  w = display.getWidth();
  h = display.getHeight();
  // Top left
  point_x[0] = w / 8;
  point_y[0] = h / 8;
  // Top right
  point_x[1] = point_x[0] * 7;
  point_y[1] = point_y[0];
  // Bottom left
  point_x[2] = point_x[0];
  point_y[2] = point_y[0] * 7;
  // Bottom right
  point_x[3] = point_x[1];
  point_y[3] = point_y[2];

  display.setCursor(100, 100);
  display.setTextColor(Display::Color::Red);
  display.setFontSize(Display::FontSize::LARGE);
  display.println("Touch Calibration");
#else
  display.setHeaderHeight(HEADER_HEIGHT);
  drawHeader();
  previousButton.enabled = true;
  nextButton.enabled = true;

  //readNextSampleFrame();
#ifdef SERIAL_OUT 
  Serial.println("### Boot complete !");
#endif
#endif
}

void loop()
{
  #ifndef TOUCH_CAL
  //Serial.print(".");
  if(display.touchAvailable())
  {
    int x = display.getTouchX();
    int y = display.getTouchY();
    if(x>=0 && y>=0)
    {
      //Serial.println(x);
      //Serial.println(y);
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
      else if((y <= HEADER_BUTTON_BOTTOM)&&(x >= HEADER_BUTTON_LEFT)&&(x <= HEADER_BUTTON_RIGHT))
      {
        //Serial.print("Read frame #");
        //Serial.println(curFrame);
        readNextSampleFrame();
      }
    }
  }
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
    {
      frameReceivedLedBlink();
      readBeacon();
      updateDisplay();
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
  #else  // Touch screen calibration
  if (current_point != next_point)
  {
    current_point = next_point;
    display.setColor(Display::Color::Red);
    display.drawLine(
        point_x[current_point] - 5,
        point_y[current_point] - 5,
        point_x[current_point] + 5,
        point_y[current_point] + 5
        );
    display.drawLine(
        point_x[current_point] + 5,
        point_y[current_point] - 5,
        point_x[current_point] - 5,
        point_y[current_point] + 5
        );
  }

  if (display.touchAvailable())
  {
    int32_t total_x = display.getTouchX(), total_y = display.getTouchY();
    int count = 1;
    while (display.touchAvailable())
    {
      total_x += display.getTouchX();
      total_y += display.getTouchY();
      count++;
      // Serial.printf("ctouch_raw_x: %d, ctouch_raw_y: %d, count: %d\n", ctouch_raw_x, ctouch_raw_y, count);
    }
    touched_x[current_point] = total_x / count;
    touched_y[current_point] = total_y / count;
    // Serial.printf("touched_x: %d, touched_y: %d\n", touched_x[current_point], touched_y[current_point]);

    if (current_point == 3)
    {
      Serial.print("touched_x[0]: ");Serial.print(touched_x[0]);Serial.print(" touched_y[0]: ");Serial.println(touched_y[0]);
      Serial.print("touched_x[1]: ");Serial.print(touched_x[1]);Serial.print(" touched_y[1]: ");Serial.println(touched_y[1]);
      Serial.print("touched_x[2]: ");Serial.print(touched_x[2]);Serial.print(" touched_y[2]: ");Serial.println(touched_y[2]);
      Serial.print("touched_x[3]: ");Serial.print(touched_x[3]);Serial.print(" touched_y[3]: ");Serial.println(touched_y[3]);
      uint16_t delta_x = (touched_x[0] > touched_x[1]) ? (touched_x[0] - touched_x[1]) : (touched_x[1] - touched_x[0]);
      uint16_t delta_y = (touched_y[0] > touched_y[1]) ? (touched_y[0] - touched_y[1]) : (touched_y[1] - touched_y[0]);

      if (delta_x > delta_y)
      {
        ctouch_swap_xy = false;
        ctouch_map_x1 = (touched_x[0] + touched_x[2]) / 2;
        ctouch_map_x2 = (touched_x[1] + touched_x[3]) / 2;
        ctouch_map_y1 = (touched_y[0] + touched_y[1]) / 2;
        ctouch_map_y2 = (touched_y[2] + touched_y[3]) / 2;
      }
      else
      {
        ctouch_swap_xy = true;
        ctouch_map_x1 = (touched_y[0] + touched_y[2]) / 2;
        ctouch_map_x2 = (touched_y[1] + touched_y[3]) / 2;
        ctouch_map_y1 = (touched_x[0] + touched_x[1]) / 2;
        ctouch_map_y2 = (touched_x[2] + touched_x[3]) / 2;
      }

      if (ctouch_map_x1 > ctouch_map_x2)
      {
        delta_x = (ctouch_map_x1 - ctouch_map_x2) / 6;
        ctouch_map_x1 += delta_x;
        ctouch_map_x2 -= delta_x;
      }
      else
      {
        delta_x = (ctouch_map_x2 - ctouch_map_x1) / 6;
        ctouch_map_x1 -= delta_x;
        ctouch_map_x2 += delta_x;
      }

      if (ctouch_map_y1 > ctouch_map_y2)
      {
        delta_y = (ctouch_map_y1 - ctouch_map_y2) / 6;
        ctouch_map_y1 += delta_y;
        ctouch_map_y2 -= delta_y;
      }
      else
      {
        delta_y = (ctouch_map_y2 - ctouch_map_y1) / 6;
        ctouch_map_y1 -= delta_y;
        ctouch_map_y2 += delta_y;
      }

      Serial.print("bool ctouch_swap_xy = ");Serial.println(ctouch_swap_xy ? "true" : "false");
      Serial.print("int16_t ctouch_map_x1 = ");Serial.println(ctouch_map_x1);
      Serial.print("int16_t ctouch_map_x2 = ");Serial.println(ctouch_map_x2);
      Serial.print("int16_t ctouch_map_y1 = ");Serial.println(ctouch_map_y1);
      Serial.print("int16_t ctouch_map_y2 = ");Serial.println(ctouch_map_y2);

      // wait next touch to continue
      while (display.touchAvailable())
        ;

      display.clearDisplay(false);
      display.setCursor(100, 100);
      display.setTextColor(Display::Color::Red);
      display.setFontSize(Display::FontSize::LARGE);
      display.println("Touch Calibration");
    }

    next_point = (current_point == 3) ? 0 : (current_point + 1);
  }

  delay(100);  
  #endif
}


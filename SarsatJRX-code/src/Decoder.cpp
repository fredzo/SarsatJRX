#include "Decoder.h"

unsigned long microseconds;
unsigned long modDuration;
int frameStartCount = 0;
bool frameComplete = false;
bool lastModState = 1; // Start with 1
bool currentBitValue = 1; // Start with 1
byte frameParseState = 0;
byte bitCount;
byte currentByte;
unsigned long frameStartTime;
bool frameStarted = false;
byte frame[Beacon::SIZE];
byte byteCount;

#ifdef DEBUG_DECODE
  // For debug
  int events[512];
  int eventCount = 0;
  int* getEvents() {return events;}
  int getEventCount() {return eventCount;}
#endif

bool isFrameStarted()
{
  return frameStarted;
}

byte* getFrame()
{
  return frame;
}

byte getByteCount()
{
  return byteCount;
}

unsigned long getFrameStartTime()
{
  return frameStartTime;
}

bool isFrameComplete()
{
  return frameComplete;
}

void setFrameComplete(bool complete)
{
  frameComplete = complete;
}

void readFrameBits()
{ 
  // Frame format :
  // First Byte / Second Byte = Bit synchronization = 0xFF+0xFE
  // Third Byte = Frame synchrnization = 0xD0 for normal message / 0x2F for self-test message
  // First bit of fourth byte indicates the message format : 0 = short message (11 more bytes, i.e. 14 bytes in total) / 1 = long message (15 more bytes, i.e. 18 bytes in total)
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
      { // TODO : handle different byte count for small messages
        frameComplete = true; 
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
  frameComplete = false;
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
  Read incoming bits
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
      readFrameBits();
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
    if(frameComplete)
    { // Frame complete, ignore further bits
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
        readFrameBits();
      }
      lastModState = !lastModState;
    }
    else if(modDuration >= CHANGE_PHASE_START && modDuration < CHANGE_PHASE_END)
    { // Phase change => bit value change
      currentBitValue = !currentBitValue;
      // Store new bit and lauch parsing
      currentByte = currentByte << 1 | currentBitValue;
      lastModState = 0;
      readFrameBits();
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
        readFrameBits();
      }
      bool odd = ((missedModulation%2) != 0);
      if(odd) 
      { // We have a phase change
        currentBitValue = !currentBitValue;
        // Store new bit and lauch parsing
        currentByte = currentByte << 1 | currentBitValue;
        lastModState = 0;
        readFrameBits();
      }
    }
  }
}


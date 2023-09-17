#include "Util.h"


#ifdef DEBUG_RAM
uint32_t freeRam()
{
  return ESP.getFreeHeap();
}
void logFreeRam()
{
   Serial.printf("Free ram : %d\n",ESP.getFreeHeap());
}
#endif


/**
 * @brief Convert the provided frame to it's Hex String representation
 * 
 * @param frame the frame to convert
 * @param withSpace true if bytes should be seperated by spaces
 * @param start start byte
 * @param end end byte
 * @return String 
 */
String toHexString(byte* frame, bool withSpace, int start, int end)
{
  char buffer[4];
  String result = "";
  for ( byte i = start; i < end; i++) 
  {
    sprintf(buffer, "%02X", frame[i]);
    if(withSpace && i>start) 
    {
      result += " ";
    }
    result += buffer;
  }
  return result;
}

/**
 * @brief Read the provided hexString and store the content in the buffer
 * 
 * @param buffer the buffer to store the converted hex bytes into (the size must match the hex string size)
 * @param hexString the hex string to read
 */
void readHexString(byte* buffer, String hexString) 
{
  for (unsigned int i = 0; i < hexString.length(); i += 2) {
    String byteString = hexString.substring(i, i+2);
    byte b = (byte)strtol(byteString.c_str(), NULL, 16);
    buffer[i/2]=b;
  }
}

String formatMemoryValue(uint32_t value, bool showByteValue)
{
  char buffer[64];
  uint32_t kbValue = value / 1024;
  if(showByteValue)
  {
    sprintf(buffer,"%'d kB (%'d bytes)",kbValue,value);
  }
  else
  {
    sprintf(buffer,"%'d kB",kbValue);
  }
  return String(buffer);
}

String formatFrequencyValue(uint32_t value)
{
  return (String(value) + " Mhz");
}

String formatHzFrequencyValue(uint32_t value)
{
  return (String(value/1000000) + " Mhz");
}



/* Baudot code matrix */
char BAUDOT_CODE[64]   = {' ','5',' ','9',' ',' ',' ',' ',' ',' ','4',' ','8','0',' ',' ',
                          '3',' ',' ',' ',' ','6',' ','/','-','2',' ',' ','7','1',' ',' ',
                          ' ','T',' ','O',' ','H','N','M',' ','L','R','G','I','P','C','V',
                          'E','Z','D','B','S','Y','F','X','A','W','J',' ','U','Q','K','\0'};

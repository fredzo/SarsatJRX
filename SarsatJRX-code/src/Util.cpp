#include "Util.h"

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


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


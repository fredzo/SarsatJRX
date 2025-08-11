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
String toHexString(volatile byte* frame, bool withSpace, int start, int end)
{
  char buffer[4];
  String result = "";
  for ( byte i = start; i < end; i++) 
  {
    snprintf(buffer,sizeof(buffer), "%02X", frame[i]);
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
void readHexString(volatile byte* buffer, String hexString) 
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
    snprintf(buffer,sizeof(buffer),"%'d kB (%'d bytes)",kbValue,value);
  }
  else
  {
    snprintf(buffer,sizeof(buffer),"%'d kB",kbValue);
  }
  return String(buffer);
}

String formatFrequencyValue(uint32_t value)
{
  return (String(value) + " Mhz");
}

String formatDbmValue(int8_t value)
{
  return (String(value) + " dBm");
}

String formatHzFrequencyValue(uint32_t value)
{
  return (String(value/1000000) + " Mhz");
}

String formatSketchInformation(uint32_t size, String md5)
{
  int md5Length = md5.length();
  //Serial.println(md5);
  if(md5Length > 16)
  {
    md5 = md5.substring(0,8) + "..." + md5.substring(md5Length-8,md5Length);
    //Serial.println(md5);
  }
  return formatMemoryValue(size,false) + " (MD5=" + md5 + ")";
}

void formatBeaconFileName(char* buffer, size_t size, String name)
{
  snprintf(buffer,size,"%s/%s/%s - %s:%s:%s",name.substring(0,2),name.substring(2,4),name.substring(4,8),name.substring(9,11),name.substring(11,13),name.substring(13,15));
}

Rtc::Date parseBeaconFileName(const char* fileName)
{
  String name = String(fileName);
  Rtc::Date result;
  result.day = atoi(name.substring(0,2).c_str());
  result.month = atoi(name.substring(2,4).c_str());
  result.year = atoi(name.substring(4,8).c_str());
  result.hour = atoi(name.substring(9,11).c_str());
  result.minute = atoi(name.substring(11,13).c_str());
  result.second = atoi(name.substring(13,15).c_str());
  return result;
}

void formatFrequencyItem(char* buffer, size_t size, int index, float frequency, bool on)
{
    snprintf(buffer,size,"(%c) %d- %3.4f MHz",on ? '*' : ' ', index+1, frequency);
}

typedef struct {
    float voltage;
    uint8_t percent;
} voltageMap_t;

// Simplified battery discharge curve
typedef struct {
    float relVoltage;   // Relative voltage (from 0.0 to 1.0)
    uint8_t percent;
} battery_point_t;

// Normalized discharge curve
static const battery_point_t batteryCurve[] = {
    {1.00, 100},
    {0.98,  95},
    {0.95,  90},
    {0.90,  80},
    {0.85,  70},
    {0.80,  60},
    {0.75,  50},
    {0.70,  40},
    {0.65,  30},
    {0.60,  20},
    {0.55,  10},
    {0.50,   5},
    {0.00,   0}
};

uint8_t voltageToPercent(float voltage, float voltageMin, float voltageMax) 
{
    if (voltageMax <= voltageMin) return 0;

    // Ratio entre 0.0 (min) et 1.0 (max)
    float norm = (voltage - voltageMin) / (voltageMax - voltageMin);

    const int count = sizeof(batteryCurve) / sizeof(batteryCurve[0]);

    if (norm >= batteryCurve[0].relVoltage) return 100;
    if (norm <= batteryCurve[count - 1].relVoltage) return 0;

    for (int i = 0; i < count - 1; i++) {
        float r_high = batteryCurve[i].relVoltage;
        float r_low  = batteryCurve[i + 1].relVoltage;
        uint8_t p_high = batteryCurve[i].percent;
        uint8_t p_low  = batteryCurve[i + 1].percent;

        if (norm <= r_high && norm >= r_low) 
        {
            float percent = p_low + (norm - r_low) * (p_high - p_low) / (r_high - r_low);
            return (uint8_t)(percent + 0.5f);
        }
    }
    return 0;
}

/* Baudot code matrix */
char BAUDOT_CODE[64]   = {' ','5',' ','9',' ',' ',' ',' ',' ',' ','4',' ','8','0',' ',' ',
                          '3',' ',' ',' ',' ','6',' ','/','-','2',' ',' ','7','1',' ',' ',
                          ' ','T',' ','O',' ','H','N','M',' ','L','R','G','I','P','C','V',
                          'E','Z','D','B','S','Y','F','X','A','W','J',' ','U','Q','K','\0'};

#include "Power.h"

/**
 * @brief Read 1.1 volts internal reference
 * 
 * @return unsigned int 
 */
unsigned int analogReadReference(void) 
{  
  /* Clear any remaining charge */
#if defined(__AVR_ATmega328P__)
  ADMUX = 0x4F;
#elif defined(__AVR_ATmega2560__)
  ADCSRB &= ~(1 << MUX5);
  ADMUX = 0x5F;
#elif defined(__AVR_ATmega32U4__)
  ADCSRB &= ~(1 << MUX5);
  ADMUX = 0x5F;
#endif
  delayMicroseconds(5);
  
  /*Select 1.1 volts internal reference as a measuring point, with VCC as a hight limit */
#if defined(__AVR_ATmega328P__)
  ADMUX = 0x4E;
#elif defined(__AVR_ATmega2560__)
  ADCSRB &= ~(1 << MUX5);
  ADMUX = 0x5E;
#elif defined(__AVR_ATmega32U4__)
  ADCSRB &= ~(1 << MUX5);
  ADMUX = 0x5E;
#endif
  delayMicroseconds(200);

  /* Activate ADC  */
  ADCSRA |= (1 << ADEN);
  
  /* Start conversion */
  ADCSRA |= (1 << ADSC);
  
  /* Wait for conversion to end */
  while(ADCSRA & (1 << ADSC));
  
  /* Read conversion result */
  return ADCL | (ADCH << 8);
}

/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float getVccValue() {
  
  /* 
   * VCC is about 5 volts for 1023
   * Internal reference is 1.1 volts = (1023 * 1.1) / 5 = 225
   * 
   * Mesuring référence voltage at 1.1 volts allows detuction of th actual power of the contoller
   * VCC = (1023 * 1.1) / analogReadReference()
   */
  float result = (1023 * 1.1) / analogReadReference();
  return result;
}

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @param vccString the char* to store the VCC string value in
 */
void getVccStringValue(char* vccString)
{   /* 6 is mininum width, 4 is precision; float value is copied onto angleStr */
    dtostrf(getVccValue(), 3, 2, vccString);
}

